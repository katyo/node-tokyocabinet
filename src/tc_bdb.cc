#include "tc.hh"

namespace node {
  extern "C" {
#include <tcbdb.h>
  }


  class BDB : public TCWrap {
  public:
    TCBDB *bdb;

    const static Persistent<FunctionTemplate> Tmpl;

    BDB () {
      bdb = tcbdbnew();
    }

    ~BDB () {
      tcbdbdel(bdb);
    }

    static void
    Initialize (const Handle<Object> target) {
      HandleScope scope;
      //set_ecodes(Tmpl);
      Tmpl->InstanceTemplate()->SetInternalFieldCount(1);

      DEFINE_PREFIXED_CONSTANT(Tmpl, BDB, TLARGE);
      DEFINE_PREFIXED_CONSTANT(Tmpl, BDB, TDEFLATE);
      DEFINE_PREFIXED_CONSTANT(Tmpl, BDB, TBZIP);
      DEFINE_PREFIXED_CONSTANT(Tmpl, BDB, TTCBS);
      DEFINE_PREFIXED_CONSTANT(Tmpl, BDB, OREADER);
      DEFINE_PREFIXED_CONSTANT(Tmpl, BDB, OWRITER);
      DEFINE_PREFIXED_CONSTANT(Tmpl, BDB, OCREAT);
      DEFINE_PREFIXED_CONSTANT(Tmpl, BDB, OTRUNC);
      DEFINE_PREFIXED_CONSTANT(Tmpl, BDB, ONOLCK);
      DEFINE_PREFIXED_CONSTANT(Tmpl, BDB, OLCKNB);
      DEFINE_PREFIXED_CONSTANT(Tmpl, BDB, OTSYNC);

      NODE_SET_PROTOTYPE_METHOD(Tmpl, "errmsg", ErrmsgSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "ecode", EcodeSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "setmutex", SetmutexSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "tune", TuneSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "setcache", SetcacheSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "setxmsiz", SetxmsizSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "setdfunit", SetdfunitSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "openSync", OpenSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "openAsync", OpenAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "closeSync", CloseSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "closeAsync", CloseAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "putSync", PutSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "putAsync", PutAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "putkeepSync", PutkeepSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "putkeepAsync", PutkeepAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "putcatSync", PutcatSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "putcatAsync", PutcatAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "putdupSync", PutdupSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "putdupAsync", PutdupAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "putlistSync", PutlistSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "putlistAsync", PutlistAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "outSync", OutSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "outAsync", OutAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "outlistSync", OutlistSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "outlistAsync", OutlistAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "getSync", GetSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "getAsync", GetAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "getlistSync", GetlistSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "getlistAsync", GetlistAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "vnumSync", VnumSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "vnumAsync", VnumAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "vsizSync", VsizSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "vsizAsync", VsizAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "rangeSync", RangeSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "rangeAsync", RangeAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "fwmkeysSync", FwmkeysSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "fwmkeysAsync", FwmkeysAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "addintSync", AddintSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "addintAsync", AddintAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "adddoubleSync", AdddoubleSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "adddoubleAsync", AdddoubleAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "syncSync", SyncSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "syncAsync", SyncAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "optimizeSync", OptimizeSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "optimizeAsync", OptimizeAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "vanishSync", VanishSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "vanishAsync", VanishAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "copySync", CopySync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "copyAsync", CopyAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "tranbeginSync", TranbeginSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "tranbeginAsync", TranbeginAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "trancommitSync", TrancommitSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "trancommitAsync", TrancommitAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "tranabortSync", TranabortSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "tranabortAsync", TranabortAsync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "path", PathSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "rnum", RnumSync);
      NODE_SET_PROTOTYPE_METHOD(Tmpl, "fsiz", FsizSync);

      target->Set(String::New("BDB"), Tmpl->GetFunction());
    }

  private:

    static Handle<Value>
    New (const Arguments& args) {
      HandleScope scope;
      if (!args.IsConstructCall()) return args.Callee()->NewInstance();
      (new BDB)->Wrap(THIS);
      return THIS;
    }

    int Ecode () {
      return tcbdbecode(bdb);
    }

    DEFINE_SYNC2(Ecode);

    const char * Errmsg (int ecode) {
      return tcbdberrmsg(ecode);
    }

    DEFINE_SYNC2(Errmsg);

    bool Setmutex () {
      return tcbdbsetmutex(bdb);
    }

    DEFINE_SYNC(Setmutex);

    virtual bool Tune (int32_t lmemb, int32_t nmemb, int64_t bnum, int8_t apow, 
                       int8_t fpow, uint8_t opts) {
      return tcbdbtune(bdb, lmemb, nmemb, bnum, apow, fpow, opts);
    }

    class TuneData : public virtual ArgsData {
    protected:
      int32_t lmemb;
      int32_t nmemb;
      int64_t bnum;
      int8_t apow;
      int8_t fpow;
      uint8_t opts;

    public:
      static bool checkArgs (const Arguments& args) {
        return (args[0]->IsNumber() || NOU(args[0])) &&
          (args[1]->IsNumber() || NOU(args[1])) &&
          (args[2]->IsNumber() || NOU(args[2])) &&
          (args[3]->IsNumber() || NOU(args[3])) &&
          (args[4]->IsNumber() || NOU(args[4])) &&
          (args[5]->IsNumber() || NOU(args[5]));
      }

      TuneData (const Arguments& args) : ArgsData(args) {
        lmemb = NOU(args[0]) ? -1 : args[0]->Int32Value();
        nmemb = NOU(args[1]) ? -1 : args[1]->Int32Value();
        bnum = NOU(args[2]) ? -1 : args[2]->IntegerValue();
        apow = NOU(args[3]) ? -1 : args[3]->Int32Value();
        fpow = NOU(args[4]) ? -1 : args[4]->Int32Value();
        opts = NOU(args[5]) ? UINT8_MAX : args[5]->Int32Value();
      }

      bool run () {
        return tcw->Tune(lmemb, nmemb, bnum, apow, fpow, opts);
      }
    };

    DEFINE_SYNC(Tune);

    bool Setcache (int32_t lcnum, int32_t ncnum) {
      return tcbdbsetcache(bdb, lcnum, ncnum);
    }

    class SetcacheData : public ArgsData {
    private:
      int32_t lcnum;
      int32_t ncnum;

    public:
      SetcacheData (const Arguments& args) : ArgsData(args) {
        lcnum = args[0]->IsUndefined() ? -1 : args[0]->Int32Value();
        ncnum = args[1]->IsUndefined() ? -1 : args[1]->Int32Value();
      }

      static bool
      checkArgs (const Arguments& args) {
        return (args[0]->IsNumber() || args[0]->IsUndefined()) &&
          (args[1]->IsNumber() || args[1]->IsUndefined());
      }

      bool
      run () {
        return tcw->Setcache(lcnum, ncnum);
      }
    };

    DEFINE_SYNC(Setcache);

    bool Setxmsiz (int64_t xmsiz) {
      return tcbdbsetxmsiz(bdb, xmsiz);
    }

    DEFINE_SYNC(Setxmsiz);

    bool Setdfunit (int32_t dfunit) {
      return tcbdbsetdfunit(bdb, dfunit);
    }

    DEFINE_SYNC(Setdfunit);

    bool Open (char *path, int omode) {
      return tcbdbopen(bdb, path, omode);
    }

    class OpenData : public FilenameData {
    protected:
      int omode;

    public:
      OpenData (const Arguments& args) : FilenameData(args), ArgsData(args) {
        omode = NOU(args[1]) ? BDBOREADER : args[1]->Int32Value();
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
      return tcbdbclose(bdb);
    }

    DEFINE_SYNC(Close);
    DEFINE_ASYNC(Close);

    bool Put(char *kbuf, int ksiz, char *vbuf, int vsiz) {
      return tcbdbput(bdb, kbuf, ksiz, vbuf, vsiz);
    }

    DEFINE_SYNC(Put);
    DEFINE_ASYNC(Put);

    bool Putkeep(char *kbuf, int ksiz, char *vbuf, int vsiz) {
      return tcbdbputkeep(bdb, kbuf, ksiz, vbuf, vsiz);
    }

    DEFINE_SYNC(Putkeep);
    DEFINE_ASYNC(Putkeep);

    bool Putcat(char *kbuf, int ksiz, char *vbuf, int vsiz) {
      return tcbdbputcat(bdb, kbuf, ksiz, vbuf, vsiz);
    }

    DEFINE_SYNC(Putcat);
    DEFINE_ASYNC(Putcat);

    bool Putdup(char *kbuf, int ksiz, char *vbuf, int vsiz) {
      return tcbdbputdup(bdb, kbuf, ksiz, vbuf, vsiz);
    }

    DEFINE_SYNC(Putdup);
    DEFINE_ASYNC(Putdup);

    bool Putlist(char *kbuf, int ksiz, const TCLIST *vals) {
      return tcbdbputdup3(bdb, kbuf, ksiz, vals);
    }

    DEFINE_SYNC(Putlist);
    DEFINE_ASYNC(Putlist);

    bool Out(char *kbuf, int ksiz) {
      return tcbdbout(bdb, kbuf, ksiz);
    }

    DEFINE_SYNC(Out);
    DEFINE_ASYNC(Out);

    bool Outlist(char *kbuf, int ksiz) {
      return tcbdbout3(bdb, kbuf, ksiz);
    }

    DEFINE_SYNC(Outlist);
    DEFINE_ASYNC(Outlist);

    char * Get(char *kbuf, int ksiz, int *vsiz_p) {
      return static_cast<char *>(tcbdbget(bdb, kbuf, ksiz, vsiz_p));
    }

    DEFINE_SYNC2(Get);
    DEFINE_ASYNC2(Get);

    TCLIST * Getlist(char *kbuf, int ksiz) {
      return tcbdbget4(bdb, kbuf, ksiz);
    }

    DEFINE_SYNC2(Getlist);
    DEFINE_ASYNC2(Getlist);

    int Vnum(char *kbuf, int ksiz) {
      return tcbdbvnum(bdb, kbuf, ksiz);
    }

    DEFINE_SYNC2(Vnum);
    DEFINE_ASYNC2(Vnum);

    int Vsiz(char *kbuf, int ksiz) {
      return tcbdbvsiz(bdb, kbuf, ksiz);
    }

    DEFINE_SYNC2(Vsiz);
    DEFINE_ASYNC2(Vsiz);

    TCLIST * Range(char *bkbuf, int bksiz, bool binc, char *ekbuf, int eksiz, 
                   bool einc, int max) {
      return tcbdbrange(bdb, bkbuf, bksiz, binc, ekbuf, eksiz, einc, max);
    }

    class RangeData : public virtual ArgsData {
    protected:
      String::Utf8Value bkbuf;
      int bksiz;
      bool binc;
      String::Utf8Value ekbuf;
      int eksiz;
      bool einc;
      int max;
      TCLIST *list;

    public:
      static bool
      checkArgs (const Arguments& args) {
        return (NOU(args[1]) || args[1]->IsBoolean()) &&
          (NOU(args[3]) || args[3]->IsBoolean()) &&
          (NOU(args[4]) || args[4]->IsNumber());
      }

      RangeData (const Arguments& args) 
      : bkbuf(args[0]), ekbuf(args[2]), ArgsData(args) {
        bksiz = args[0]->IsNull() ? -1 : bkbuf.length();
        binc = args[1]->BooleanValue();
        eksiz = args[2]->IsNull() ? -1 : ekbuf.length();
        einc = args[3]->BooleanValue();
        max = NOU(args[4]) ? -1 : args[4]->Int32Value();
      }

      ~RangeData () {
        tclistdel(list);
      }

      bool
      run () {
        list = tcw->Range(bksiz == -1 ? NULL : *bkbuf, bksiz, binc,
                          eksiz == -1 ? NULL : *ekbuf, eksiz, einc, max);
        return true;
      }

      Handle<Value>
      returnValue () {
        HandleScope scope;
        return scope.Close(tclisttoary(list));
      }
    };

    class RangeAsyncData : public RangeData, public AsyncData {
    public:
      RangeAsyncData (const Arguments& args)
      : RangeData(args), AsyncData(args[5]), ArgsData(args) {}
    };

    DEFINE_SYNC2(Range);
    DEFINE_ASYNC2(Range);

    TCLIST * Fwmkeys(char *kbuf, int ksiz, int max) {
      return tcbdbfwmkeys(bdb, kbuf, ksiz, max);
    }

    DEFINE_SYNC2(Fwmkeys);
    DEFINE_ASYNC2(Fwmkeys);

    int Addint(char *kbuf, int ksiz, int num) {
      return tcbdbaddint(bdb, kbuf, ksiz, num);
    }

    DEFINE_SYNC2(Addint);
    DEFINE_ASYNC2(Addint);

    double Adddouble(char *kbuf, int ksiz, double num) {
      return tcbdbadddouble(bdb, kbuf, ksiz, num);
    }

    DEFINE_SYNC2(Adddouble);
    DEFINE_ASYNC2(Adddouble);

    bool Sync () {
      return tcbdbsync(bdb);
    }

    DEFINE_SYNC(Sync);
    DEFINE_ASYNC(Sync);

    bool Optimize (int32_t lmemb, int32_t nmemb, int64_t bnum, int8_t apow, 
                   int8_t fpow, uint8_t opts) {
      return tcbdboptimize(bdb, lmemb, nmemb, bnum, apow, fpow, opts);
    }

    class OptimizeData : public TuneData {
    public:
      OptimizeData (const Arguments& args) : TuneData(args), ArgsData(args) {}

      bool run () {
        return tcw->Optimize(lmemb, nmemb, bnum, apow, fpow, opts);
      }
    };

    DEFINE_SYNC(Optimize);

    class OptimizeAsyncData : public OptimizeData, public AsyncData {
    public:
      OptimizeAsyncData (const Arguments& args)
      : OptimizeData(args), AsyncData(args[6]), ArgsData(args) {}
    };

    DEFINE_ASYNC(Optimize);

    bool Vanish () {
      return tcbdbvanish(bdb);
    }

    DEFINE_SYNC(Vanish);
    DEFINE_ASYNC(Vanish);

    bool Copy (char *path) {
      return tcbdbcopy(bdb, path);
    }

    DEFINE_SYNC(Copy);
    DEFINE_ASYNC(Copy);

    bool Tranbegin () {
      return tcbdbtranbegin(bdb);
    }

    DEFINE_SYNC(Tranbegin);
    DEFINE_ASYNC(Tranbegin);

    bool Trancommit () {
      return tcbdbtrancommit(bdb);
    }

    DEFINE_SYNC(Trancommit);
    DEFINE_ASYNC(Trancommit);

    bool Tranabort () {
      return tcbdbtranabort(bdb);
    }

    DEFINE_SYNC(Tranabort);
    DEFINE_ASYNC(Tranabort);

    const char * Path () {
      return tcbdbpath(bdb);
    }

    DEFINE_SYNC2(Path)

    uint64_t Rnum () {
      return tcbdbrnum(bdb);
    }

    DEFINE_SYNC2(Rnum);

    uint64_t Fsiz () {
      return tcbdbfsiz(bdb);
    }

    DEFINE_SYNC2(Fsiz);
  };

  const Persistent<FunctionTemplate> BDB::Tmpl =
    Persistent<FunctionTemplate>::New(FunctionTemplate::New(BDB::New));

  class CUR : TCWrap {
  public:
    CUR (TCBDB *bdb) {
      cur = tcbdbcurnew(bdb);
    }

    ~CUR () {
      tcbdbcurdel(cur);
    }

    static CUR *
    Unwrap (const Handle<Object> obj) {
      return ObjectWrap::Unwrap<CUR>(obj);
    }

    static BDBCUR *
    Backend (const Handle<Object> obj) {
      return Unwrap(obj)->cur;
    }

    static void
    Initialize (const Handle<Object> target) {
      HandleScope scope;
      Local<FunctionTemplate> tmpl = FunctionTemplate::New(New);

      Local<ObjectTemplate> ot = tmpl->InstanceTemplate();
      ot->SetInternalFieldCount(1);

      DEFINE_PREFIXED_CONSTANT(tmpl, BDB, CPCURRENT);
      DEFINE_PREFIXED_CONSTANT(tmpl, BDB, CPBEFORE);
      DEFINE_PREFIXED_CONSTANT(tmpl, BDB, CPAFTER);

      NODE_SET_PROTOTYPE_METHOD(tmpl, "firstSync", FirstSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "firstAsync", FirstAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "lastSync", LastSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "lastAsync", LastAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "jumpSync", JumpSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "jumpAsync", JumpAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "prevSync", PrevSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "prevAsync", PrevAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "nextSync", NextSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "nextAsync", NextAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "putSync", PutSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "putAsync", PutAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "outSync", OutSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "outAsync", OutAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "keySync", KeySync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "keyAsync", KeyAsync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "valSync", ValSync);
      NODE_SET_PROTOTYPE_METHOD(tmpl, "valAsync", ValAsync);

      target->Set(String::New("BDBCUR"), tmpl->GetFunction());
    }

  private:
    BDBCUR *cur;

    static Handle<Value>
    New (const Arguments& args) {
      HandleScope scope;
      if (args.Length() < 1 ||
          !BDB::Tmpl->HasInstance(args[0])) {
        return THROW_BAD_ARGS;
      }
      TCBDB *bdb = ObjectWrap::Unwrap<BDB>(Local<Object>::Cast(args[0]))->bdb;
      (new CUR(bdb))->Wrap(THIS);
      return THIS;
    }

    int Ecode () {
      return tcbdbecode(cur->bdb);
    }

    bool First () {
      return tcbdbcurfirst(cur);
    }

    class FirstData : public virtual ArgsData {
    public:
      FirstData(const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->First();
      }
    };

    DEFINE_SYNC(First);

    class FirstAsyncData : public FirstData, public AsyncData {
    public:
      FirstAsyncData(const Arguments& args)
      : AsyncData(args[0]), FirstData(args), ArgsData(args) {}
    };

    DEFINE_ASYNC(First);

    bool Last () {
      return tcbdbcurlast(cur);
    }

    class LastData : public virtual ArgsData {
    public:
      LastData(const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Last();
      }
    };

    DEFINE_SYNC(Last);

    class LastAsyncData : public LastData, public AsyncData {
    public:
      LastAsyncData(const Arguments& args)
      : AsyncData(args[0]), LastData(args), ArgsData(args) {}
    };

    DEFINE_ASYNC(Last);

    bool Jump (char *kbuf, int ksiz) {
      return tcbdbcurjump(cur, kbuf, ksiz);
    }

    class JumpData : public KeyData {
    public:
      JumpData(const Arguments& args) : KeyData(args), ArgsData(args) {}

      bool run () {
        return tcw->Jump(*kbuf, ksiz);
      }
    };

    DEFINE_SYNC(Jump);

    class JumpAsyncData : public JumpData, public AsyncData {
    public:
      JumpAsyncData(const Arguments& args)
      : AsyncData(args[1]), JumpData(args), ArgsData(args) {}
    };

    DEFINE_ASYNC(Jump);

    bool Prev () {
      return tcbdbcurprev(cur);
    }

    class PrevData : public virtual ArgsData {
    public:
      PrevData(const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Prev();
      }
    };

    DEFINE_SYNC(Prev);

    class PrevAsyncData : public PrevData, public AsyncData {
    public:
      PrevAsyncData(const Arguments& args)
      : AsyncData(args[0]), PrevData(args), ArgsData(args) {}
    };

    DEFINE_ASYNC(Prev);

    bool Next () {
      return tcbdbcurnext(cur);
    }

    class NextData : public virtual ArgsData {
    public:
      NextData(const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Next();
      }
    };

    DEFINE_SYNC(Next);

    class NextAsyncData : public NextData, public AsyncData {
    public:
      NextAsyncData(const Arguments& args)
      : AsyncData(args[0]), NextData(args), ArgsData(args) {}
    };

    DEFINE_ASYNC(Next);

    bool Put (char *kbuf, int ksiz, int cpmode) {
      return tcbdbcurput(cur, kbuf, ksiz, cpmode);
    }

    class PutData : public KeyData {
    private:
      int cpmode;

    public:
      PutData(const Arguments& args) : KeyData(args), ArgsData(args) {
        cpmode = NOU(args[1]) ?
          BDBCPCURRENT : args[1]->Int32Value();
      }

      static bool checkArgs (const Arguments& args) {
        return NOU(args[1]) || args[1]->IsNumber();
      }

      bool run () {
        return tcw->Put(*kbuf, ksiz, cpmode);
      }
    };

    DEFINE_SYNC(Put);

    class PutAsyncData : public PutData, public AsyncData {
    public:
      PutAsyncData(const Arguments& args)
      : AsyncData(args[2]), PutData(args), ArgsData(args) {}
    };

    DEFINE_ASYNC(Put);

    class OutData : public virtual ArgsData {
    public:
      OutData(const Arguments& args) : ArgsData(args) {}

      bool run () {
        return tcw->Out();
      }
    };

    bool Out () {
      return tcbdbcurout(cur);
    }

    DEFINE_SYNC(Out);

    class OutAsyncData : public OutData, public AsyncData {
    public:
      OutAsyncData(const Arguments& args)
      : AsyncData(args[0]), OutData(args), ArgsData(args) {}
    };

    DEFINE_ASYNC(Out);

    char * Key (int *vsiz_p) {
      return static_cast<char *>(tcbdbcurkey(cur, vsiz_p));
    }

    class KeyData : public ValueData {
    public:
      KeyData(const Arguments& args) : ArgsData(args) {}

      bool run () {
        vbuf = tcw->Key(&vsiz);
        return vbuf != NULL;
      }
    };

    DEFINE_SYNC2(Key);

    class KeyAsyncData : public KeyData, public AsyncData {
    public:
      KeyAsyncData(const Arguments& args)
      : AsyncData(args[0]), KeyData(args), ArgsData(args) {}
    };

    DEFINE_ASYNC2(Key);

    char * Val (int *vsiz_p) {
      return static_cast<char *>(tcbdbcurval(cur, vsiz_p));
    }

    class ValData : public ValueData {
    public:
      ValData(const Arguments& args) : ArgsData(args) {}

      bool run () {
        vbuf = tcw->Val(&vsiz);
        return vbuf != NULL;
      }
    };

    DEFINE_SYNC2(Val);

    class ValAsyncData : public ValData, public AsyncData {
    public:
      ValAsyncData(const Arguments& args)
      : AsyncData(args[0]), ValData(args), ArgsData(args) {}
    };

    DEFINE_ASYNC2(Val);
  };

  MODULE_INIT(bdb, CLASS_INIT(BDB) CLASS_INIT(CUR));
}
