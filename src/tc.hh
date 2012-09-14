#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <v8.h>

namespace node {
  using namespace v8;
  
  extern "C" {
#include <tcutil.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <tctdb.h>
  }

#define THROW_BAD_ARGS                                                  \
  ThrowException(Exception::TypeError(String::New("Bad arguments")))
  
#define DEFINE_PREFIXED_CONSTANT(target, prefix, constant)      \
  (target)->Set(String::NewSymbol(#constant),                   \
                Integer::New(prefix##constant))

#define THIS args.This()

  // null or undefined
#define NOU(obj) ((obj)->IsNull() || (obj)->IsUndefined())

  // conversion between Tokyo Cabinet list/map to V8 Arrya/Object and vice versa
  inline TCLIST* arytotclist (const Handle<Array> ary) {
    HandleScope scope;
    int len = ary->Length();
    TCLIST *list = tclistnew2(len);
    Handle<Value> val;
    for (int i = 0; i < len; i++) {
      val = ary->Get(Integer::New(i));
      if (val->IsString()) {
        tclistpush2(list, *String::Utf8Value(val));
      }
    }
    return list;
  }

  inline Local<Value> tclisttoary (TCLIST *list) {
    HandleScope scope;
    if(list != NULL){
      int len = tclistnum(list);
      Local<Array> ary = Array::New(len);
      for (int i = 0; i < len; i++) {
        ary->Set(Integer::New(i), String::New(tclistval2(list, i)));
      }
      return scope.Close(ary);
    }else{
      return scope.Close(Null());
    }
  }

  inline TCMAP* objtotcmap (const Handle<Object> obj) {
    HandleScope scope;
    TCMAP *map = tcmapnew2(31);
    Local<Array> keys = obj->GetPropertyNames();
    int len = keys->Length();
    Local<Value> key, val;
    for (int i = 0; i < len; i++) {
      key = keys->Get(Integer::New(i));
      val = obj->Get(key);
      if (NOU(val)) continue;
      String::Utf8Value u8key(key);
      String::Utf8Value u8val(val);
      tcmapput(map, *u8key, u8key.length(), *u8val, u8val.length());
    }
    return map;
  }

  inline Local<Value> tcmaptoobj (TCMAP *map) {
    HandleScope scope;
    if(map != NULL){
      const char *kbuf, *vbuf;
      int ksiz, vsiz;
      Local<Object> obj = Object::New();
      tcmapiterinit(map);
      for (;;) {
        kbuf = static_cast<const char*>(tcmapiternext(map, &ksiz));
        if (kbuf == NULL) break;
        vbuf = static_cast<const char*>(tcmapiterval(kbuf, &vsiz));
        obj->Set(String::New(kbuf, ksiz), String::New(vbuf, vsiz));
      }
      return scope.Close(obj);
    }else{
      return scope.Close(Null());
    }
  }

  /* sync method blueprint */
#define DEFINE_SYNC(name)                                       \
  static Handle<Value>                                          \
  name##Sync (const Arguments& args) {                          \
    HandleScope scope;                                          \
    if (!name##Data::checkArgs(args)) {                         \
      return THROW_BAD_ARGS;                                    \
    }                                                           \
    return scope.Close(Boolean::New(name##Data(args).run()));   \
  }

  /* when there is an extra value to return */
#define DEFINE_SYNC2(name)                      \
  static Handle<Value>                          \
  name##Sync (const Arguments& args) {          \
    HandleScope scope;                          \
    if (!name##Data::checkArgs(args)) {         \
      return THROW_BAD_ARGS;                    \
    }                                           \
    name##Data data(args);                      \
    data.run();                                 \
    return scope.Close(data.returnValue());     \
  }

  /* async method blueprint */
#define DEFINE_ASYNC_FUNC(name)                                 \
  static Handle<Value>                                          \
  name##Async (const Arguments& args) {                         \
    HandleScope scope;                                          \
    if (!name##Data::checkArgs(args)) {                         \
      return THROW_BAD_ARGS;                                    \
    }                                                           \
    name##AsyncData *data = new name##AsyncData(args);          \
    eio_custom(Exec##name, EIO_PRI_DEFAULT, After##name, data); \
    ev_ref(EV_DEFAULT_UC);                                      \
    return Undefined();                                         \
  }

#define DEFINE_ASYNC_EXEC(name)                                         \
  static void                                                           \
  Exec##name (eio_req *req) {                                           \
    name##AsyncData *data = static_cast<name##AsyncData *>(req->data);  \
    req->result = data->run() ? TCESUCCESS : data->ecode();             \
  }

#define DEFINE_ASYNC_AFTER(name)                                        \
  static int                                                            \
  After##name (eio_req *req) {                                          \
    HandleScope scope;                                                  \
    name##AsyncData *data = static_cast<name##AsyncData *>(req->data);  \
    if (data->hasCallback) {                                            \
      data->callCallback(Integer::New(req->result));                    \
    }                                                                   \
    ev_unref(EV_DEFAULT_UC);                                            \
    delete data;                                                        \
    return 0;                                                           \
  }

  /* when there is an extra value to return */
#define DEFINE_ASYNC_AFTER2(name)                                       \
  static int                                                            \
  After##name (eio_req *req) {                                          \
    HandleScope scope;                                                  \
    name##AsyncData *data = static_cast<name##AsyncData *>(req->data);  \
    if (data->hasCallback) {                                            \
      data->callCallback(Integer::New(req->result), data->returnValue()); \
    }                                                                   \
    ev_unref(EV_DEFAULT_UC);                                            \
    delete data;                                                        \
    return 0;                                                           \
  }

#define DEFINE_ASYNC(name)                      \
  DEFINE_ASYNC_FUNC(name)                       \
  DEFINE_ASYNC_EXEC(name)                       \
    DEFINE_ASYNC_AFTER(name)

#define DEFINE_ASYNC2(name)                     \
  DEFINE_ASYNC_FUNC(name)                       \
  DEFINE_ASYNC_EXEC(name)                       \
    DEFINE_ASYNC_AFTER2(name)

#define CLASS_INIT(name)                        \
  name::Initialize(target);

#define ADDON_INIT(name, classes)                                       \
  void                                                                  \
  init_##name (Handle<Object> target) {                                 \
    classes;                                                            \
    set_ecodes (target);                                                \
    target->Set(String::NewSymbol("VERSION"), String::New(tcversion));  \
  }                                                                     \
  NODE_MODULE(name, init_##name);

#define CPART_INIT(name, classes)               \
  void                                          \
  init_##name (Handle<Object> target) {         \
    classes;                                    \
  }

#define SUBMD_INIT(name)                        \
  init_##name(target);

#define SUBMD_DECL(name)                        \
  void                                          \
  init_##name(Handle<Object> target);

#ifdef MODULE_MULTI
#define MODULE_INIT ADDON_INIT
#else
#define MODULE_INIT CPART_INIT
#endif

  // Tokyo Cabinet error codes
  void set_ecodes (const Handle<Object> tmpl);

  // Database wrapper (interfaces for database objects, all included)
  class TCWrap : public ObjectWrap {
  public:
    // these methods must be overridden in individual DB classes
    virtual int Ecode () { assert(false); }
    virtual const char * Errmsg (int ecode) { assert(false); }
    virtual bool Setmutex () { assert(false); }
    virtual bool Tune (int64_t bnum, int8_t apow, int8_t fpow, uint8_t opts) { assert(false); } // for HDB
    virtual bool Tune (int32_t lmemb, int32_t nmemb, int64_t bnum, int8_t apow, int8_t fpow, uint8_t opts) { assert(false); } // for BDB
    virtual bool Tune (int32_t width, int64_t limsiz) { assert(false); } // for FDB
    virtual bool Setcache (int32_t rcnum) { assert(false); } // for HDB
    virtual bool Setcache (int32_t lcnum, int32_t ncnum) { assert(false); } // for BDB
    virtual bool Setcache (int32_t rcnum, int32_t lcnum, int32_t ncnum) { assert(false); } // for TDB
    virtual bool Setxmsiz (int64_t xmsiz) { assert(false); }
    virtual bool Setdfunit (int32_t dfunit) { assert(false); }
    virtual bool Open (char *path, int omode) { assert(false); }
    virtual bool Open (char *path) { assert(false); } // for ADB
    virtual bool Close () { assert(false); }
    virtual bool Put(char *kbuf, int ksiz, char *vbuf, int vsiz) { assert(false); }
    virtual bool Put(char *kbuf, int ksiz, TCMAP *map) { assert(false); } // for TDB
    virtual bool Putkeep(char *kbuf, int ksiz, char *vbuf, int vsiz) { assert(false); }
    virtual bool Putkeep(char *kbuf, int ksiz, TCMAP *map) { assert(false); } // for TDB
    virtual bool Putcat(char *kbuf, int ksiz, char *vbuf, int vsiz) { assert(false); }
    virtual bool Putcat(char *kbuf, int ksiz, TCMAP *map) { assert(false); } // for TDB
    virtual bool Putasync(char *kbuf, int ksiz, char *vbuf, int vsiz) { assert(false); }
    virtual bool Putdup(char *kbuf, int ksiz, char *vbuf, int vsiz) { assert(false); } // for BDB
    virtual bool Putlist(char *kbuf, int ksiz, TCLIST *list) { assert(false); } // for BDB
    virtual bool Out(char *kbuf, int ksiz) { assert(false); }
    virtual bool Outlist(char *kbuf, int ksiz) { assert(false); } // for BDB
    virtual char * Get(char *kbuf, int ksiz, int *vsiz_p) { assert(false); }
    virtual TCMAP * Get(char *kbuf, int ksiz) { assert(false); } // for TDB
    virtual TCLIST * Getlist(char *kbuf, int ksiz) { assert(false); } // for BDB
    virtual int Vnum (char *kbuf, int ksiz) { assert(false); }
    virtual int Vsiz(char *kbuf, int ksiz) { assert(false); }
    virtual TCLIST * Range(char *bkbuf, int bksiz, bool binc, char *ekbuf, int eksiz, bool einc, int max) { assert(false); } // for BDB
    virtual TCLIST * Range(char *ibuf, int isiz, int max) { assert(false); } // for FDB
    virtual bool Iterinit () { assert(false); }
    virtual char * Iternext (int *vsiz_p) { assert(false); }
    virtual TCLIST * Fwmkeys(char *kbuf, int ksiz, int max) { assert(false); }
    virtual int Addint(char *kbuf, int ksiz, int num) { assert(false); }
    virtual double Adddouble(char *kbuf, int ksiz, double num) { assert(false); }
    virtual bool Sync () { assert(false); }
    virtual bool Optimize (int64_t bnum, int8_t apow, int8_t fpow, uint8_t opts) { assert(false); } // for HDB
    virtual bool Optimize (int32_t lmemb, int32_t nmemb, int64_t bnum, int8_t apow, int8_t fpow, uint8_t opts) { assert(false); } // for HDB
    virtual bool Optimize (int32_t width, int64_t limsiz) { assert(false); } // for FDB
    virtual bool Optimize (const char *params) { assert(false); } // for ADB
    virtual bool Vanish () { assert(false); }
    virtual bool Copy (char *path) { assert(false); }
    virtual bool Tranbegin () { assert(false); }
    virtual bool Trancommit () { assert(false); }
    virtual bool Tranabort () { assert(false); }
    virtual const char* Path () { assert(false); }
    virtual uint64_t Rnum () { assert(false); }
    virtual uint64_t Fsiz () { assert(false); }
    virtual uint64_t Size () { assert(false); } // for ADB
    virtual bool Setindex (const char* name, int type) { assert(false); } // for TDB
    virtual TCLIST * Misc (const char *name, TCLIST *targs) { assert(false); } // for ADB
    // for BDB Cursor
    virtual bool First () { assert(false); } // for CUR
    virtual bool Last () { assert(false); } // for CUR
    virtual bool Jump (char *kbuf, int ksiz) { assert(false); } // for CUR
    virtual bool Prev () { assert(false); } // for CUR
    virtual bool Next () { assert(false); } // for CUR
    virtual bool Put (char *kbuf, int ksiz, int cpmode) { assert(false); } // for CUR
    virtual bool Out () { assert(false); } // for CUR
    virtual char * Key (int *vsiz_p) { assert(false); } // for CUR
    virtual char * Val (int *vsiz_p) { assert(false); } // for CUR
    // for TDB Query
    virtual TCLIST * Search () { assert(false); } // for QRY
    virtual bool Searchout () { assert(false); } // for QRY
    virtual TCLIST * Metasearch (TDBQRY **qrys, int num, int type) { assert(false); } // for QRY
  protected:
    class ArgsData {
    protected:
      TCWrap *tcw;

      /* This is defined only because AsyncData(Handle<Value>) constructor 
       * gives compile error without this, but in fact AsyncData does not
       * use it. */
      ArgsData () {
        assert(false);
      }

    public:
      static bool 
      checkArgs (const Arguments& args) {
        return true;
      }

      ArgsData (const Arguments& args) {
        tcw = Unwrap<TCWrap>(args.This());
      }

      int
      ecode () {
        return tcw->Ecode();
      }
    };

    class AsyncData : public virtual ArgsData {
    public:
      Persistent<Function> cb;
      bool hasCallback;

      AsyncData (Handle<Value> cb_) {
        HandleScope scope;
        assert(tcw); // make sure ArgsData is already initialized with This value
        tcw->Ref();
        if (cb_->IsFunction()) {
          hasCallback = true;
          cb = Persistent<Function>::New(Handle<Function>::Cast(cb_));
        } else {
          hasCallback = false;
        }
      }

      virtual 
      ~AsyncData () {
        tcw->Unref();
        cb.Dispose();
      }

      inline void
      callback (int argc, Handle<Value> argv[]) {
        TryCatch try_catch;
        cb->Call(Context::GetCurrent()->Global(), argc, argv);
        if (try_catch.HasCaught()) {
          FatalException(try_catch);
        }
      }

      inline void
      callCallback (Handle<Value> arg0) {
        HandleScope scope;
        Handle<Value> args[1] = {arg0};
        callback(1, args);
      }

      inline void
      callCallback (Handle<Value> arg0, Handle<Value>arg1) {
        HandleScope scope;
        Handle<Value> args[2] = {arg0, arg1};
        callback(2, args);
      }
    };

    class EcodeData : public ArgsData {
    private:
      int ecode;

    public:
      EcodeData (const Arguments& args) : ArgsData(args) {}

      bool run () {
        ecode = tcw->Ecode();
        return true;
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        return scope.Close(Integer::New(ecode));
      }
    };

    class ErrmsgData : public ArgsData {
    public:
      int ecode;
      const char *msg;

      ErrmsgData (const Arguments& args) : ArgsData(args) {
        ecode = args[0]->IsNumber() ? args[0]->Int32Value() : tcw->Ecode();
      }

      bool 
      run () {
        msg = tcw->Errmsg(ecode);
        return true;
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        return scope.Close(String::New(msg));
      }

      static bool 
      checkArgs (const Arguments& args) {
        return args[0]->IsNumber() || args[0]->IsUndefined();
      }
    };

    class SetmutexData : public ArgsData {
    public:
      SetmutexData (const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Setmutex();
      }
    };

    class SetxmsizData : public ArgsData {
    private:
      int64_t xmsiz;

    public:
      SetxmsizData (const Arguments& args) : ArgsData(args) {
        xmsiz = args[0]->IsUndefined() ? -1 : args[0]->IntegerValue();
      }

      static bool
      checkArgs (const Arguments& args) {
        return args[0]->IsNumber() || args[0]->IsUndefined();
      }

      bool
      run () {
        return tcw->Setxmsiz(xmsiz);
      }
    };

    class SetdfunitData : public ArgsData {
    private:
      int32_t dfunit;

    public:
      SetdfunitData (const Arguments& args) : ArgsData(args) {
        dfunit = args[0]->IsUndefined() ? -1 : args[0]->Int32Value();
      }

      static bool
      checkArgs (const Arguments& args) {
        return args[0]->IsNumber() || args[0]->IsUndefined();
      }

      bool
      run () {
        return tcw->Setdfunit(dfunit);
      }
    };

    class FilenameData : public virtual ArgsData {
    protected:
      String::Utf8Value path;

    public:
      FilenameData (const Arguments& args) : path(args[0]), ArgsData(args) {}

      static bool
      checkArgs (const Arguments& args) {
        return args[0]->IsString();
      }
    };

    class CloseData : public virtual ArgsData {
    public:
      CloseData (const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Close();
      }
    };

    class CloseAsyncData : public CloseData, public AsyncData {
    public:
      CloseAsyncData (const Arguments& args)
      : CloseData(args), AsyncData(args[0]), ArgsData(args) {}
    };

    class KeyData : public virtual ArgsData {
    protected:
      String::Utf8Value kbuf;
      int ksiz;

    public:
      KeyData (const Arguments& args) : kbuf(args[0]), ArgsData(args) {
        ksiz = kbuf.length();
      }
    };

    class PutData : public KeyData {
    protected:
      String::Utf8Value vbuf;
      int vsiz;

    public:
      PutData (const Arguments& args) 
      : vbuf(args[1]), KeyData(args), ArgsData(args) {
        vsiz = vbuf.length();
      }

      bool
      run () {
        return tcw->Put(*kbuf, ksiz, *vbuf, vsiz);
      }
    };

    class PutAsyncData : public PutData, public AsyncData {
    public:
      PutAsyncData (const Arguments& args)
      : PutData(args), AsyncData(args[2]), ArgsData(args) {}
    };

    class PutkeepData : public PutData {
    public:
      PutkeepData (const Arguments& args) : PutData(args), ArgsData(args) {}

      bool
      run () {
        return tcw->Putkeep(*kbuf, ksiz, *vbuf, vsiz);
      }
    };

    class PutkeepAsyncData : public PutkeepData, public AsyncData {
    public:
      PutkeepAsyncData (const Arguments& args)
      : PutkeepData(args), AsyncData(args[2]), ArgsData(args) {}
    };

    class PutcatData : public PutData {
    public:
      PutcatData (const Arguments& args) : PutData(args), ArgsData(args) {}

      bool
      run () {
        return tcw->Putcat(*kbuf, ksiz, *vbuf, vsiz);
      }
    };

    class PutcatAsyncData : public PutcatData, public AsyncData {
    public:
      PutcatAsyncData (const Arguments& args)
      : PutcatData(args), AsyncData(args[2]), ArgsData(args) {}
    };

    class PutasyncData : public PutData {
    public:
      PutasyncData (const Arguments& args) : PutData(args), ArgsData(args) {}

      bool
      run () {
        return tcw->Putasync(*kbuf, ksiz, *vbuf, vsiz);
      }
    };

    class PutasyncAsyncData : public PutasyncData, public AsyncData {
    public:
      PutasyncAsyncData (const Arguments& args)
      : PutasyncData(args), AsyncData(args[2]), ArgsData(args) {}
    };

    class PutdupData : public PutData {
    public:
      PutdupData (const Arguments& args) : PutData(args), ArgsData(args) {}

      bool
      run () {
        return tcw->Putdup(*kbuf, ksiz, *vbuf, vsiz);
      }
    };

    class PutdupAsyncData : public PutdupData, public AsyncData {
    public:
      PutdupAsyncData (const Arguments& args)
      : PutdupData(args), AsyncData(args[2]), ArgsData(args) {}
    };

    class PutlistData : public KeyData {
    private:
      TCLIST *list;

    public:
      PutlistData (const Arguments& args) : KeyData(args), ArgsData(args) {
        HandleScope scope;
        list = arytotclist(Handle<Array>::Cast(args[0]));
      }

      ~PutlistData () {
        tclistdel(list);
      }

      bool
      run () {
        return tcw->Putlist(*kbuf, ksiz, list);
      }
    };

    class PutlistAsyncData : public PutlistData, public AsyncData {
    public:
      PutlistAsyncData (const Arguments& args)
      : PutlistData(args), AsyncData(args[2]), ArgsData(args) {}
    };

    class ValueData : public virtual ArgsData {
    protected:
      char *vbuf;
      int vsiz;

    public:
      ~ValueData () {
        tcfree(vbuf);
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        return vbuf == NULL ? Null() : scope.Close(String::New(vbuf, vsiz));
      }
    };

    class OutData : public KeyData {
    public:
      OutData (const Arguments& args) : KeyData(args), ArgsData(args) {}

      bool
      run () {
        return tcw->Out(*kbuf, ksiz);
      }
    };

    class OutAsyncData : public OutData, public AsyncData {
    public:
      OutAsyncData (const Arguments& args)
      : OutData(args), AsyncData(args[1]), ArgsData(args) {}
    };

    class OutlistData : public KeyData {
    public:
      OutlistData (const Arguments& args) : KeyData(args), ArgsData(args) {}

      bool
      run () {
        return tcw->Outlist(*kbuf, ksiz);
      }
    };

    class OutlistAsyncData : public OutlistData, public AsyncData {
    public:
      OutlistAsyncData (const Arguments& args)
      : OutlistData(args), AsyncData(args[1]), ArgsData(args) {}
    };

    class GetData : public KeyData, public ValueData {
    public:
      GetData (const Arguments& args) : KeyData(args), ArgsData(args) {}

      bool
      run () {
        vbuf = tcw->Get(*kbuf, ksiz, &vsiz);
        return vbuf != NULL;
      }
    };

    class GetAsyncData : public GetData, public AsyncData {
    public:
      GetAsyncData (const Arguments& args)
      : GetData(args), AsyncData(args[1]), ArgsData(args) {}
    };

    class GetlistData : public KeyData {
    protected:
      TCLIST *list;

    public:
      GetlistData (const Arguments& args) : KeyData(args), ArgsData(args) {}

      ~GetlistData () {
        tclistdel(list);
      }

      bool
      run () {
        list = tcw->Getlist(*kbuf, ksiz);
        return true;
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        return scope.Close(tclisttoary(list));
      }
    };

    class GetlistAsyncData : public GetlistData, public AsyncData {
    public:
      GetlistAsyncData (const Arguments& args)
      : GetlistData(args), AsyncData(args[1]), ArgsData(args) {}
    };

    class FwmkeysData : public GetlistData {
    protected:
      int max;

    public:
      FwmkeysData (const Arguments& args) : GetlistData(args), ArgsData(args) {
        max = args[1]->Int32Value();
      }

      bool
      run () {
        list = tcw->Fwmkeys(*kbuf, ksiz, max);
        return true;
      }

      static bool
      checkArgs (const Arguments& args) {
        return args[1]->IsNumber();
      }
    };

    class FwmkeysAsyncData : public FwmkeysData, public AsyncData {
    public:
      FwmkeysAsyncData (const Arguments& args)
      : FwmkeysData(args), AsyncData(args[2]), ArgsData(args) {}
    };

    class VnumData : public KeyData {
    protected:
      int vnum;

    public:
      VnumData (const Arguments& args) : KeyData(args), ArgsData(args) {}

      bool
      run () {
        vnum = tcw->Vnum(*kbuf, ksiz);
        return vnum != 0;
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        return scope.Close(Number::New(vnum));
      }
    };

    class VnumAsyncData : public VnumData, public AsyncData {
    public:
      VnumAsyncData (const Arguments& args)
      : VnumData(args), AsyncData(args[1]), ArgsData(args) {}
    };

    class VsizData : public KeyData {
    protected:
      int vsiz;

    public:
      VsizData (const Arguments& args) : KeyData(args), ArgsData(args) {}

      bool
      run () {
        vsiz = tcw->Vsiz(*kbuf, ksiz);
        return vsiz != -1;
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        return scope.Close(Number::New(vsiz));
      }
    };

    class VsizAsyncData : public VsizData, public AsyncData {
    public:
      VsizAsyncData (const Arguments& args)
      : VsizData(args), AsyncData(args[1]), ArgsData(args) {}
    };

    class AddintData : public KeyData {
    protected:
      int num;

    public:
      AddintData (const Arguments& args) : KeyData(args), ArgsData(args) {
        num = args[1]->Int32Value();
      }

      static bool
      checkArgs (const Arguments& args) {
        return args[1]->IsNumber();
      }

      bool
      run () {
        num = tcw->Addint(*kbuf, ksiz, num);
        return num != INT_MIN;
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        return num == INT_MIN ? Null() : scope.Close(Integer::New(num));
      }
    };

    class AddintAsyncData : public AddintData, public AsyncData {
    public:
      AddintAsyncData (const Arguments& args)
      : AddintData(args), AsyncData(args[2]), ArgsData(args) {}
    };

    class IterinitData : public virtual ArgsData {
    public:
      IterinitData (const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Iterinit();
      }
    };

    class IterinitAsyncData : public IterinitData, public AsyncData {
    public:
      IterinitAsyncData (const Arguments& args)
      : IterinitData(args), AsyncData(args[0]), ArgsData(args) {}
    };

    class IternextData : public ValueData {
    public:
      IternextData (const Arguments& args) : ArgsData(args){}

      bool run () {
        vbuf = tcw->Iternext(&vsiz);
        return vbuf != NULL;
      }
    };

    class IternextAsyncData : public IternextData, public AsyncData {
    public:
      IternextAsyncData (const Arguments& args)
      : IternextData(args), AsyncData(args[0]), ArgsData(args) {}
    };

    class AdddoubleData : public KeyData {
    protected:
      double num;

    public:
      AdddoubleData (const Arguments& args) : KeyData(args), ArgsData(args) {
        num = args[1]->NumberValue();
      }

      static bool
      checkArgs (const Arguments& args) {
        return args[1]->IsNumber();
      }

      bool
      run () {
        num = tcw->Adddouble(*kbuf, ksiz, num);
        return isnan(num);
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        return isnan(num) ? Null() : scope.Close(Number::New(num));
      }
    };

    class AdddoubleAsyncData : public AdddoubleData, public AsyncData {
    public:
      AdddoubleAsyncData (const Arguments& args)
      : AdddoubleData(args), AsyncData(args[2]), ArgsData(args) {}
    };

    class SyncData : public virtual ArgsData {
    public:
      SyncData (const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Sync();
      }
    };

    class SyncAsyncData : public SyncData, public AsyncData {
    public:
      SyncAsyncData (const Arguments& args)
      : SyncData(args), AsyncData(args[0]), ArgsData(args) {}
    };

    class VanishData : public virtual ArgsData {
    public:
      VanishData (const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Vanish();
      }
    };

    class VanishAsyncData : public VanishData, public AsyncData {
    public:
      VanishAsyncData (const Arguments& args)
      : VanishData(args), AsyncData(args[0]), ArgsData(args) {}
    };

    class CopyData : public FilenameData {
    public:
      CopyData (const Arguments& args) : FilenameData(args), ArgsData(args) {}

      bool
      run () {
        return tcw->Copy(*path);
      }
    };

    class CopyAsyncData : public CopyData, public AsyncData {
    public:
      CopyAsyncData (const Arguments& args)
      : CopyData(args), AsyncData(args[2]), ArgsData(args) {}
    };

    class TranbeginData : public virtual ArgsData {
    public:
      TranbeginData (const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Tranbegin();
      }
    };

    class TranbeginAsyncData : public TranbeginData, public AsyncData {
    public:
      TranbeginAsyncData (const Arguments& args)
      : TranbeginData(args), AsyncData(args[0]), ArgsData(args) {}
    };

    class TrancommitData : public virtual ArgsData {
    public:
      TrancommitData (const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Trancommit();
      }
    };

    class TrancommitAsyncData : public TrancommitData, public AsyncData {
    public:
      TrancommitAsyncData (const Arguments& args)
      : TrancommitData(args), AsyncData(args[0]), ArgsData(args) {}
    };

    class TranabortData : public virtual ArgsData {
    public:
      TranabortData (const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Tranabort();
      }
    };

    class TranabortAsyncData : public TranabortData, public AsyncData {
    public:
      TranabortAsyncData (const Arguments& args)
      : TranabortData(args), AsyncData(args[0]), ArgsData(args) {}
    };

    class PathData : public ArgsData {
    private:
      const char *path;

    public:
      PathData (const Arguments& args) : ArgsData(args) {}

      bool
      run () {
        path = tcw->Path();
        return path != NULL;
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        return path == NULL ? Null() : scope.Close(String::New(path));
      }
    };

    class RnumData : public ArgsData {
    private:
      uint64_t rnum;

    public:
      RnumData (const Arguments& args) : ArgsData(args) {}

      bool
      run () {
        rnum = tcw->Rnum();
        // rnum == 0 when not connected to any database file
        return rnum != 0;
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        // JavaScript integer cannot express uint64
        // so this can't handle too large number
        return scope.Close(Integer::New(rnum));
      }
    };

    class FsizData : public virtual ArgsData {
    protected:
      uint64_t fsiz;

    public:
      FsizData (const Arguments& args) : ArgsData(args) {}

      bool
      run () {
        fsiz = tcw->Fsiz();
        // fsiz == 0 when not connected to any database file
        return fsiz != 0;
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        // JavaScript integer cannot express uint64
        // so this can't handle too large number
        return scope.Close(Integer::New(fsiz));
      }
    };
  };
}
