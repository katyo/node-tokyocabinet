#include "tc.hh"

namespace node {
  extern "C" {
#include <tcfdb.h>
  }

  class FDB : public TCWrap {
  public:
    FDB () {
      fdb = tcfdbnew();
    }

    ~FDB () {
      tcfdbdel(fdb);
    }

    static void
    Initialize (const Handle<Object> target) {
      HandleScope scope;
      Local<FunctionTemplate> tmpl = FunctionTemplate::New(New);
      tmpl->InstanceTemplate()->SetInternalFieldCount(1);
      //set_ecodes(tmpl);

      DEFINE_PREFIXED_CONSTANT(tmpl, FDB, OREADER);
      DEFINE_PREFIXED_CONSTANT(tmpl, FDB, OWRITER);
      DEFINE_PREFIXED_CONSTANT(tmpl, FDB, OCREAT);
      DEFINE_PREFIXED_CONSTANT(tmpl, FDB, OTRUNC);
      DEFINE_PREFIXED_CONSTANT(tmpl, FDB, ONOLCK);
      DEFINE_PREFIXED_CONSTANT(tmpl, FDB, OLCKNB);
      DEFINE_PREFIXED_CONSTANT(tmpl, FDB, OTSYNC);

      NODE_SET_PROTOTYPE_METHOD(tmpl, "errmsg", ErrmsgSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "ecode", EcodeSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "setmutex", SetmutexSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "tune", TuneSync);
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
      NODE_SET_PROTOTYPE_METHOD(tmpl, "rangeSync", RangeSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "rangeAsync", RangeAsync);
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
      NODE_SET_PROTOTYPE_METHOD(tmpl, "fsiz", FsizSync);

      target->Set(String::New("FDB"), tmpl->GetFunction());
    }

  private:
    TCFDB *fdb;

    static Handle<Value>
    New (const Arguments& args) {
      HandleScope scope;
      if (!args.IsConstructCall()) return args.Callee()->NewInstance();
      (new FDB)->Wrap(THIS);
      return THIS;
    }

    int Ecode () {
      return tcfdbecode(fdb);
    }

    DEFINE_SYNC2(Ecode);

    const char * Errmsg (int ecode) {
      return tcfdberrmsg(ecode);
    }

    DEFINE_SYNC2(Errmsg);

    bool Setmutex () {
      return tcfdbsetmutex(fdb);
    }

    DEFINE_SYNC(Setmutex);

    bool Tune (int32_t width, int64_t limsiz) {
      return tcfdbtune(fdb, width, limsiz);
    }

    class TuneData : public virtual ArgsData {
    protected:
      int32_t width;
      int64_t limsiz;

    public:
      static bool checkArgs (const Arguments& args) {
        return (args[0]->IsNumber() || NOU(args[0])) &&
          (args[1]->IsNumber() || NOU(args[1]));
      }

      TuneData (const Arguments& args) : ArgsData(args) {
        width = NOU(args[0]) ? -1 : args[0]->Int32Value();
        limsiz = NOU(args[1]) ? -1 : args[1]->IntegerValue();
      }

      bool run () {
        return tcw->Tune(width, limsiz);
      }
    };

    DEFINE_SYNC(Tune);

    bool Open (char *path, int omode) {
      return tcfdbopen(fdb, path, omode);
    }

    class OpenData : public FilenameData {
    protected:
      int omode;

    public:
      OpenData (const Arguments& args) : FilenameData(args), ArgsData(args) {
        omode = NOU(args[1]) ? FDBOREADER : args[1]->Int32Value();
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
      return tcfdbclose(fdb);
    }

    DEFINE_SYNC(Close);
    DEFINE_ASYNC(Close);

    bool Put(char *kbuf, int ksiz, char *vbuf, int vsiz) {
      return tcfdbput2(fdb, kbuf, ksiz, vbuf, vsiz);
    }

    DEFINE_SYNC(Put);
    DEFINE_ASYNC(Put);

    bool Putkeep(char *kbuf, int ksiz, char *vbuf, int vsiz) {
      return tcfdbputkeep2(fdb, kbuf, ksiz, vbuf, vsiz);
    }

    DEFINE_SYNC(Putkeep);
    DEFINE_ASYNC(Putkeep);

    bool Putcat(char *kbuf, int ksiz, char *vbuf, int vsiz) {
      return tcfdbputcat2(fdb, kbuf, ksiz, vbuf, vsiz);
    }

    DEFINE_SYNC(Putcat);
    DEFINE_ASYNC(Putcat);

    bool Out(char *kbuf, int ksiz) {
      return tcfdbout2(fdb, kbuf, ksiz);
    }

    DEFINE_SYNC(Out);
    DEFINE_ASYNC(Out);

    char * Get(char *kbuf, int ksiz, int *vsiz_p) {
      return static_cast<char *>(tcfdbget2(fdb, kbuf, ksiz, vsiz_p));
    }

    DEFINE_SYNC2(Get);
    DEFINE_ASYNC2(Get);

    int Vsiz(char *kbuf, int ksiz) {
      return tcfdbvsiz2(fdb, kbuf, ksiz);
    }

    DEFINE_SYNC2(Vsiz);
    DEFINE_ASYNC2(Vsiz);

    bool Iterinit () {
      return tcfdbiterinit(fdb);
    }

    DEFINE_SYNC(Iterinit);
    DEFINE_ASYNC(Iterinit);

    char * Iternext (int *vsiz_p) {
      return static_cast<char *>(tcfdbiternext2(fdb, vsiz_p));
    }

    DEFINE_SYNC2(Iternext);
    DEFINE_ASYNC2(Iternext);

    TCLIST * Range(char *ibuf, int isiz, int max) {
      return tcfdbrange4(fdb, ibuf, isiz, max);
    }

    // Get keys with an interval notation
    // arg[0] : interval notation
    // arg[1] : maximum number of keys to be fetched
    class RangeData : public GetlistData {
    protected:
      int max;

    public:
      static bool
      checkArgs (const Arguments& args) {
        return NOU(args[1]) || args[1]->IsNumber();
      }

      RangeData (const Arguments& args) : GetlistData(args), ArgsData(args) {
        max = NOU(args[1]) ? -1 : args[1]->Int32Value();
      }

      bool
      run () {
        list = tcw->Range(*kbuf, ksiz, max);
        return true;
      }
    };

    class RangeAsyncData : public RangeData, public AsyncData {
    public:
      RangeAsyncData (const Arguments& args)
      : RangeData(args), AsyncData(args[5]), ArgsData(args) {}
    };

    DEFINE_SYNC2(Range);
    DEFINE_ASYNC2(Range);

    int Addint(char *kbuf, int ksiz, int num) {
      return tcfdbaddint(fdb, tcfdbkeytoid(kbuf, ksiz), num);
    }

    DEFINE_SYNC2(Addint);
    DEFINE_ASYNC2(Addint);

    double Adddouble(char *kbuf, int ksiz, double num) {
      return tcfdbadddouble(fdb, tcfdbkeytoid(kbuf, ksiz), num);
    }

    DEFINE_SYNC2(Adddouble);
    DEFINE_ASYNC2(Adddouble);

    bool Sync () {
      return tcfdbsync(fdb);
    }

    DEFINE_SYNC(Sync);
    DEFINE_ASYNC(Sync);

    bool Optimize (int32_t width, int64_t limsiz) {
      return tcfdboptimize(fdb, width, limsiz);
    }

    class OptimizeData : public TuneData {
    public:
      OptimizeData (const Arguments& args) : TuneData(args), ArgsData(args) {}

      bool run () {
        return tcw->Optimize(width, limsiz);
      }
    };

    DEFINE_SYNC(Optimize);

    class OptimizeAsyncData : public OptimizeData, public AsyncData {
    public:
      OptimizeAsyncData (const Arguments& args)
      : OptimizeData(args), AsyncData(args[2]), ArgsData(args) {}
    };

    DEFINE_ASYNC(Optimize);

    bool Vanish () {
      return tcfdbvanish(fdb);
    }

    DEFINE_SYNC(Vanish);
    DEFINE_ASYNC(Vanish);

    bool Copy (char *path) {
      return tcfdbcopy(fdb, path);
    }

    DEFINE_SYNC(Copy);
    DEFINE_ASYNC(Copy);

    bool Tranbegin () {
      return tcfdbtranbegin(fdb);
    }

    DEFINE_SYNC(Tranbegin);
    DEFINE_ASYNC(Tranbegin);

    bool Trancommit () {
      return tcfdbtrancommit(fdb);
    }

    DEFINE_SYNC(Trancommit);
    DEFINE_ASYNC(Trancommit);

    bool Tranabort () {
      return tcfdbtranabort(fdb);
    }

    DEFINE_SYNC(Tranabort);
    DEFINE_ASYNC(Tranabort);

    const char * Path () {
      return tcfdbpath(fdb);
    }

    DEFINE_SYNC2(Path);

    uint64_t Rnum () {
      return tcfdbrnum(fdb);
    }

    DEFINE_SYNC2(Rnum);

    uint64_t Fsiz () {
      return tcfdbfsiz(fdb);
    }

    DEFINE_SYNC2(Fsiz)
  };

  MODULE_INIT(fdb, CLASS_INIT(FDB));
}
