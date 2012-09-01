#include "tc.hh"

namespace node {
  extern "C" {
#include <tcadb.h>
  }

  class ADB : TCWrap {
  public:
    ADB () {
      adb = tcadbnew();
    }

    ~ADB () {
      tcadbdel(adb);
    }

    static void
    Initialize (const Handle<Object> target) {
      HandleScope scope;
      Local<FunctionTemplate> tmpl = FunctionTemplate::New(New);
      //set_ecodes(tmpl);
      tmpl->InstanceTemplate()->SetInternalFieldCount(1);

      NODE_SET_PROTOTYPE_METHOD(tmpl, "openSync", OpenSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "openAsync", OpenAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "closeSync", CloseSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "closeAsync", CloseAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "putSync", PutSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "putAsync", PutAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "putkeepSync", PutkeepSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "putkeepAsync", PutkeepAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "putcatSync", PutcatSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "putcatAsync", PutcatAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "outSync", OutSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "outAsync", OutAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "getSync", GetSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "getAsync", GetAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "vsizSync", VsizSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "vsizAsync", VsizAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "iterinitSync", IterinitSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "iterinitAsync", IterinitAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "iternextSync", IternextSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "iternextAsync", IternextAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "fwmkeysSync", FwmkeysSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "fwmkeysAsync", FwmkeysAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "addintSync", AddintSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "addintAsync", AddintAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "adddoubleSync", AdddoubleSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "adddoubleAsync", AdddoubleAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "syncSync", SyncSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "syncAsync", SyncAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "optimizeSync", OptimizeSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "optimizeAsync", OptimizeAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "vanishSync", VanishSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "vanishAsync", VanishAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "copySync", CopySync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "copyAsync", CopyAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "tranbeginSync", TranbeginSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "tranbeginAsync", TranbeginAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "trancommitSync", TrancommitSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "trancommitAsync", TrancommitAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "tranabortSync", TranabortSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "tranabortAsync", TranabortAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "path", PathSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "rnum", RnumSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "size", SizeSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "miscSync", MiscSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "miscAsync", MiscAsync);

      target->Set(String::New("ADB"), tmpl->GetFunction());
    }

  private:
    TCADB *adb;

    static Handle<Value>
    New (const Arguments& args) {
      HandleScope scope;
      (new ADB)->Wrap(THIS);
      return THIS;
    }

    int Ecode () {
      // TODO: return proper ecode (maybe impossible?)
      return TCEMISC;
    }

    bool Open (char *path) {
      return tcadbopen(adb, path);
    }

    class OpenData : public FilenameData {
    public:
      OpenData (const Arguments& args) : FilenameData(args), ArgsData(args) {}

      bool run () {
        return tcw->Open(*path);
      }
    };

    DEFINE_SYNC(Open);

    class OpenAsyncData : public OpenData, public AsyncData {
    public:
      OpenAsyncData (const Arguments& args)
      : OpenData(args), AsyncData(args[1]), ArgsData(args) {}
    };

    DEFINE_ASYNC(Open);

    bool Close () {
      return tcadbclose(adb);
    }

    DEFINE_SYNC(Close);
    DEFINE_ASYNC(Close);

    bool Put(char *kbuf, int ksiz, char *vbuf, int vsiz) {
      return tcadbput(adb, kbuf, ksiz, vbuf, vsiz);
    }

    DEFINE_SYNC(Put);
    DEFINE_ASYNC(Put);

    bool Putkeep(char *kbuf, int ksiz, char *vbuf, int vsiz) {
      return tcadbputkeep(adb, kbuf, ksiz, vbuf, vsiz);
    }

    DEFINE_SYNC(Putkeep);
    DEFINE_ASYNC(Putkeep);

    bool Putcat(char *kbuf, int ksiz, char *vbuf, int vsiz) {
      return tcadbputcat(adb, kbuf, ksiz, vbuf, vsiz);
    }

    DEFINE_SYNC(Putcat);
    DEFINE_ASYNC(Putcat);

    bool Out(char *kbuf, int ksiz) {
      return tcadbout(adb, kbuf, ksiz);
    }

    DEFINE_SYNC(Out);
    DEFINE_ASYNC(Out);

    char * Get(char *kbuf, int ksiz, int *vsiz_p) {
      return static_cast<char *>(tcadbget(adb, kbuf, ksiz, vsiz_p));
    }

    DEFINE_SYNC2(Get);
    DEFINE_ASYNC2(Get);

    int Vsiz(char *kbuf, int ksiz) {
      return tcadbvsiz(adb, kbuf, ksiz);
    }

    DEFINE_SYNC2(Vsiz);
    DEFINE_ASYNC2(Vsiz);

    bool Iterinit () {
      return tcadbiterinit(adb);
    }

    DEFINE_SYNC(Iterinit);
    DEFINE_ASYNC(Iterinit);

    char * Iternext (int *vsiz_p) {
      return static_cast<char *>(tcadbiternext(adb, vsiz_p));
    }

    DEFINE_SYNC2(Iternext);
    DEFINE_ASYNC2(Iternext);

    TCLIST * Fwmkeys(char *kbuf, int ksiz, int max) {
      return tcadbfwmkeys(adb, kbuf, ksiz, max);
    }

    DEFINE_SYNC2(Fwmkeys);
    DEFINE_ASYNC2(Fwmkeys);

    int Addint(char *kbuf, int ksiz, int num) {
      return tcadbaddint(adb, kbuf, ksiz, num);
    }

    DEFINE_SYNC2(Addint);
    DEFINE_ASYNC2(Addint);

    double Adddouble(char *kbuf, int ksiz, double num) {
      return tcadbadddouble(adb, kbuf, ksiz, num);
    }

    DEFINE_SYNC2(Adddouble);
    DEFINE_ASYNC2(Adddouble);

    bool Sync () {
      return tcadbsync(adb);
    }

    DEFINE_SYNC(Sync);
    DEFINE_ASYNC(Sync);

    bool Optimize (const char *params) {
      return tcadboptimize(adb, params);
    }

    class OptimizeData : public virtual ArgsData {
    protected:
      String::Utf8Value params;
      bool noparams;

    public:
      OptimizeData (const Arguments& args) : params(args[0]), ArgsData(args) {
        noparams = NOU(args[0]);
      }

      static bool checkArgs (const Arguments& args) {
        return args[0]->IsString() || NOU(args[0]);
      }

      bool run () {
        return tcw->Optimize(noparams ? NULL : *params);
      }
    };

    class OptimizeAsyncData : public OptimizeData, public AsyncData {
    public:
      OptimizeAsyncData (const Arguments& args) : 
      OptimizeData(args), AsyncData(args[1]), ArgsData(args) {}
    };

    DEFINE_SYNC(Optimize);
    DEFINE_ASYNC(Optimize);

    bool Vanish () {
      return tcadbvanish(adb);
    }

    DEFINE_SYNC(Vanish);
    DEFINE_ASYNC(Vanish);

    bool Copy (char *path) {
      return tcadbcopy(adb, path);
    }

    DEFINE_SYNC(Copy);
    DEFINE_ASYNC(Copy);

    bool Tranbegin () {
      return tcadbtranbegin(adb);
    }

    DEFINE_SYNC(Tranbegin);
    DEFINE_ASYNC(Tranbegin);

    bool Trancommit () {
      return tcadbtrancommit(adb);
    }

    DEFINE_SYNC(Trancommit);
    DEFINE_ASYNC(Trancommit);

    bool Tranabort () {
      return tcadbtranabort(adb);
    }

    DEFINE_SYNC(Tranabort);
    DEFINE_ASYNC(Tranabort);
  
    const char * Path () {
      return tcadbpath(adb);
    }

    DEFINE_SYNC2(Path)

    uint64_t Rnum () {
      return tcadbrnum(adb);
    }

    DEFINE_SYNC2(Rnum);

    uint64_t Size () {
      return tcadbsize(adb);
    }

    class SizeData : public FsizData {
    public:
      SizeData (const Arguments& args) : FsizData(args), ArgsData(args) {}

      bool run () {
        fsiz = tcw->Size();
        return fsiz != 0;
      }
    };

    DEFINE_SYNC2(Size);

    TCLIST * Misc (const char *name, TCLIST *targs) {
      return tcadbmisc(adb, name, targs);
    }

    class MiscData : public virtual ArgsData {
    protected:
      String::Utf8Value name;
      TCLIST *targs;
      TCLIST *ret;

    public:
      MiscData (const Arguments& args) : name(args[0]), ArgsData(args) {
        targs = NOU(args[1]) ? tclistnew2(1) :
          arytotclist(Local<Array>::Cast(args[1]));
      }

      ~MiscData () {
        tclistdel(targs);
        if (ret != NULL) tclistdel(ret);
      }

      static bool checkArgs (const Arguments& args) {
        return args[0]->IsString() && (NOU(args[1]) || args[1]->IsArray());
      }

      bool run () {
        ret = tcw->Misc(*name, targs);
        return ret != NULL;
      }

      Handle<Value> returnValue () {
        HandleScope scope;
        return scope.Close(ret == NULL ? Handle<Value>(Null()) : 
                           tclisttoary(ret));
      }
    };

    class MiscAsyncData : public MiscData, public AsyncData {
    public:
      MiscAsyncData (const Arguments& args) : 
      AsyncData(args[2]), MiscData(args), ArgsData(args) {}
    };

    DEFINE_SYNC2(Misc);
    DEFINE_ASYNC2(Misc);
  };

  MODULE_INIT(adb, CLASS_INIT(ADB));
}
