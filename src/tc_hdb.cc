#include "tc.hh"

extern "C" {
#include <tchdb.h>
}

class HDB : public TCWrap {
public:
  HDB () {
    hdb = tchdbnew();
  }

  ~HDB () {
    tchdbdel(hdb);
  }

  static void
  Initialize (const Handle<Object> target) {
    HandleScope scope;
    Local<FunctionTemplate> tmpl = FunctionTemplate::New(New);
    tmpl->InstanceTemplate()->SetInternalFieldCount(1);
    set_ecodes(tmpl);

    DEFINE_PREFIXED_CONSTANT(tmpl, HDB, TLARGE);
    DEFINE_PREFIXED_CONSTANT(tmpl, HDB, TDEFLATE);
    DEFINE_PREFIXED_CONSTANT(tmpl, HDB, TBZIP);
    DEFINE_PREFIXED_CONSTANT(tmpl, HDB, TTCBS);
    DEFINE_PREFIXED_CONSTANT(tmpl, HDB, OREADER);
    DEFINE_PREFIXED_CONSTANT(tmpl, HDB, OWRITER);
    DEFINE_PREFIXED_CONSTANT(tmpl, HDB, OCREAT);
    DEFINE_PREFIXED_CONSTANT(tmpl, HDB, OTRUNC);
    DEFINE_PREFIXED_CONSTANT(tmpl, HDB, ONOLCK);
    DEFINE_PREFIXED_CONSTANT(tmpl, HDB, OLCKNB);
    DEFINE_PREFIXED_CONSTANT(tmpl, HDB, OTSYNC);

    NODE_SET_PROTOTYPE_METHOD(tmpl, "errmsg", ErrmsgSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "ecode", EcodeSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "setmutex", SetmutexSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "tune", TuneSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "setcache", SetcacheSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "setxmsiz", SetxmsizSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "setdfunit", SetdfunitSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "open", OpenSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "openAsync", OpenAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "close", CloseSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "closeAsync", CloseAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "put", PutSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "putAsync", PutAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "putkeep", PutkeepSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "putkeepAsync", PutkeepAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "putcat", PutcatSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "putcatAsync", PutcatAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "putasync", PutasyncSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "putasyncAsync", PutasyncAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "out", OutSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "outAsync", OutAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "get", GetSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "getAsync", GetAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "vsiz", VsizSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "vsizAsync", VsizAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "iterinit", IterinitSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "iterinitAsync", IterinitAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "iternext", IternextSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "iternextAsync", IternextAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "fwmkeys", FwmkeysSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "fwmkeysAsync", FwmkeysAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "addint", AddintSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "addintAsync", AddintAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "adddouble", AdddoubleSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "adddoubleAsync", AdddoubleAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "sync", SyncSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "syncAsync", SyncAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "optimize", OptimizeSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "optimizeAsync", OptimizeAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "vanish", VanishSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "vanishAsync", VanishAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "copy", CopySync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "copyAsync", CopyAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "tranbegin", TranbeginSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "tranbeginAsync", TranbeginAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "trancommit", TrancommitSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "trancommitAsync", TrancommitAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "tranabort", TranabortSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "tranabortAsync", TranabortAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "path", PathSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "rnum", RnumSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "fsiz", FsizSync);

    target->Set(String::New("HDB"), tmpl->GetFunction());
  }

private:
  TCHDB *hdb;

  static Handle<Value>
  New (const Arguments& args) {
    HandleScope scope;
    if (!args.IsConstructCall()) return args.Callee()->NewInstance();
    (new HDB)->Wrap(THIS);
    return THIS;
  }

  int Ecode () {
    return tchdbecode(hdb);
  }

  DEFINE_SYNC2(Ecode);

  const char * Errmsg (int ecode) {
    return tchdberrmsg(ecode);
  }

  DEFINE_SYNC2(Errmsg);

  bool Setmutex () {
    return tchdbsetmutex(hdb);
  }

  DEFINE_SYNC(Setmutex);

  bool Setcache (int32_t rcnum) {
    return tchdbsetcache(hdb, rcnum);
  }

  class SetcacheData : public ArgsData {
  private:
    int32_t rcnum;

  public:
    SetcacheData (const Arguments& args) : ArgsData(args) {
      rcnum = args[0]->IsUndefined() ? -1 : args[0]->Int32Value();
    }

    static bool
    checkArgs (const Arguments& args) {
      return args[0]->IsNumber() || args[0]->IsUndefined();
    }

    bool
    run () {
      return tcw->Setcache(rcnum);
    }
  };

  DEFINE_SYNC(Setcache);

  bool Setxmsiz (int64_t xmsiz) {
    return tchdbsetxmsiz(hdb, xmsiz);
  }

  DEFINE_SYNC(Setxmsiz);

  bool Setdfunit (int32_t dfunit) {
    return tchdbsetdfunit(hdb, dfunit);
  }

  DEFINE_SYNC(Setdfunit);

  bool Tune (int64_t bnum, int8_t apow, int8_t fpow, uint8_t opts) {
    return tchdbtune(hdb, bnum, apow, fpow, opts);
  }

  class TuneData : public virtual ArgsData {
  protected:
    int64_t bnum;
    int8_t apow;
    int8_t fpow;
    uint8_t opts;

  public:
    static bool checkArgs (const Arguments& args) {
      return (args[0]->IsNumber() || NOU(args[0])) &&
        (args[1]->IsNumber() || NOU(args[1])) &&
        (args[2]->IsNumber() || NOU(args[2])) &&
        (args[3]->IsNumber() || NOU(args[3]));
    }

    TuneData (const Arguments& args) : ArgsData(args) {
      bnum = NOU(args[0]) ? -1 : args[0]->IntegerValue();
      apow = NOU(args[1]) ? -1 : args[1]->Int32Value();
      fpow = NOU(args[2]) ? -1 : args[2]->Int32Value();
      opts = NOU(args[3]) ? UINT8_MAX : args[3]->Int32Value();
    }

    bool run () {
      return tcw->Tune(bnum, apow, fpow, opts);
    }
  };

  DEFINE_SYNC(Tune);

  bool Open (char *path, int omode) {
    return tchdbopen(hdb, path, omode);
  }

  class OpenData : public FilenameData {
  protected:
    int omode;

  public:
    OpenData (const Arguments& args) : FilenameData(args), ArgsData(args) {
      omode = NOU(args[1]) ? HDBOREADER : args[1]->Int32Value();
    }

    static bool
    checkArgs (const Arguments& args) {
      return FilenameData::checkArgs(args) &&
        (NOU(args[1]) || args[1]->IsNumber());
    }

    bool
    run () {
      return tcw->Open(*path, omode);
    }
  };

  DEFINE_SYNC(Open);

  class OpenAsyncData : public OpenData, public AsyncData {
  public:
    OpenAsyncData (const Arguments& args)
    : OpenData(args), AsyncData(args[2]), ArgsData(args) {}
  };

  DEFINE_ASYNC(Open);

  bool Close () {
    return tchdbclose(hdb);
  }

  DEFINE_SYNC(Close);
  DEFINE_ASYNC(Close);

  bool Put(char *kbuf, int ksiz, char *vbuf, int vsiz) {
    return tchdbput(hdb, kbuf, ksiz, vbuf, vsiz);
  }

  DEFINE_SYNC(Put);
  DEFINE_ASYNC(Put);

  bool Putkeep(char *kbuf, int ksiz, char *vbuf, int vsiz) {
    return tchdbputkeep(hdb, kbuf, ksiz, vbuf, vsiz);
  }

  DEFINE_SYNC(Putkeep);
  DEFINE_ASYNC(Putkeep);

  bool Putcat(char *kbuf, int ksiz, char *vbuf, int vsiz) {
    return tchdbputcat(hdb, kbuf, ksiz, vbuf, vsiz);
  }

  DEFINE_SYNC(Putcat);
  DEFINE_ASYNC(Putcat);

  bool Putasync(char *kbuf, int ksiz, char *vbuf, int vsiz) {
    return tchdbputasync(hdb, kbuf, ksiz, vbuf, vsiz);
  }

  DEFINE_SYNC(Putasync);
  DEFINE_ASYNC(Putasync);

  bool Out(char *kbuf, int ksiz) {
    return tchdbout(hdb, kbuf, ksiz);
  }

  DEFINE_SYNC(Out);
  DEFINE_ASYNC(Out);

  char * Get(char *kbuf, int ksiz, int *vsiz_p) {
    return static_cast<char *>(tchdbget(hdb, kbuf, ksiz, vsiz_p));
  }

  DEFINE_SYNC2(Get);
  DEFINE_ASYNC2(Get);

  int Vsiz(char *kbuf, int ksiz) {
    return tchdbvsiz(hdb, kbuf, ksiz);
  }

  DEFINE_SYNC2(Vsiz);
  DEFINE_ASYNC2(Vsiz);

  bool Iterinit () {
    return tchdbiterinit(hdb);
  }

  DEFINE_SYNC(Iterinit);
  DEFINE_ASYNC(Iterinit);

  char * Iternext (int *vsiz_p) {
    return static_cast<char *>(tchdbiternext(hdb, vsiz_p));
  }

  DEFINE_SYNC2(Iternext);
  DEFINE_ASYNC2(Iternext);

  TCLIST * Fwmkeys(char *kbuf, int ksiz, int max) {
    return tchdbfwmkeys(hdb, kbuf, ksiz, max);
  }

  DEFINE_SYNC2(Fwmkeys);
  DEFINE_ASYNC2(Fwmkeys);

  int Addint(char *kbuf, int ksiz, int num) {
    return tchdbaddint(hdb, kbuf, ksiz, num);
  }

  DEFINE_SYNC2(Addint);
  DEFINE_ASYNC2(Addint);

  double Adddouble(char *kbuf, int ksiz, double num) {
    return tchdbadddouble(hdb, kbuf, ksiz, num);
  }

  DEFINE_SYNC2(Adddouble);
  DEFINE_ASYNC2(Adddouble);

  bool Sync () {
    return tchdbsync(hdb);
  }

  DEFINE_SYNC(Sync);
  DEFINE_ASYNC(Sync);

  bool Optimize (int64_t bnum, int8_t apow, int8_t fpow, uint8_t opts) {
    return tchdboptimize(hdb, bnum, apow, fpow, opts);
  }

  class OptimizeData : public TuneData {
  public:
    OptimizeData (const Arguments& args) : TuneData(args), ArgsData(args) {}

    bool run () {
      return tcw->Optimize(bnum, apow, fpow, opts);
    }
  };

  DEFINE_SYNC(Optimize);

  class OptimizeAsyncData : public OptimizeData, public AsyncData {
  public:
    OptimizeAsyncData (const Arguments& args)
    : OptimizeData(args), AsyncData(args[4]), ArgsData(args) {}
  };

  DEFINE_ASYNC(Optimize);

  bool Vanish () {
    return tchdbvanish(hdb);
  }

  DEFINE_SYNC(Vanish);
  DEFINE_ASYNC(Vanish);

  bool Copy (char *path) {
    return tchdbcopy(hdb, path);
  }

  DEFINE_SYNC(Copy);
  DEFINE_ASYNC(Copy);

  bool Tranbegin () {
    return tchdbtranbegin(hdb);
  }

  DEFINE_SYNC(Tranbegin);
  DEFINE_ASYNC(Tranbegin);

  bool Trancommit () {
    return tchdbtrancommit(hdb);
  }

  DEFINE_SYNC(Trancommit);
  DEFINE_ASYNC(Trancommit);

  bool Tranabort () {
    return tchdbtranabort(hdb);
  }

  DEFINE_SYNC(Tranabort);
  DEFINE_ASYNC(Tranabort);

  const char * Path () {
    return tchdbpath(hdb);
  }

  DEFINE_SYNC2(Path);

  uint64_t Rnum () {
    return tchdbrnum(hdb);
  }

  DEFINE_SYNC2(Rnum);

  uint64_t Fsiz () {
    return tchdbfsiz(hdb);
  }

  DEFINE_SYNC2(Fsiz);
};

MODULE_INIT(hdb, CLASS_INIT(HDB));
