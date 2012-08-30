#include "tc.hh"

extern "C" {
#include <tctdb.h>
}

class TDB : public TCWrap {
public:
  TCTDB *tdb;

  const static Persistent<FunctionTemplate> Tmpl;

  TDB () {
    tdb = tctdbnew();
  }

  ~TDB () {
    tctdbdel(tdb);
  }

  static void
  Initialize (const Handle<Object> target) {
    HandleScope scope;
    Tmpl->InstanceTemplate()->SetInternalFieldCount(1);
    set_ecodes(Tmpl);

    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, TLARGE);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, TDEFLATE);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, TBZIP);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, TTCBS);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, OREADER);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, OWRITER);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, OCREAT);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, OTRUNC);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, ONOLCK);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, OLCKNB);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, OTSYNC);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, ITLEXICAL);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, ITDECIMAL);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, ITTOKEN);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, ITQGRAM);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, ITOPT);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, ITVOID);
    DEFINE_PREFIXED_CONSTANT(Tmpl, TDB, ITKEEP);

    NODE_SET_PROTOTYPE_METHOD(Tmpl, "errmsg", ErrmsgSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "ecode", EcodeSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "setmutex", SetmutexSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "tune", TuneSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "setcache", SetcacheSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "setxmsiz", SetxmsizSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "setdfunit", SetdfunitSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "open", OpenSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "openAsync", OpenAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "close", CloseSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "closeAsync", CloseAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "put", PutSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "putAsync", PutAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "putkeep", PutkeepSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "putkeepAsync", PutkeepAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "putcat", PutcatSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "putcatAsync", PutcatAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "out", OutSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "outAsync", OutAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "get", GetSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "getAsync", GetAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "vsiz", VsizSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "vsizAsync", VsizAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "iterinit", IterinitSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "iterinitAsync", IterinitAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "iternext", IternextSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "iternextAsync", IternextAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "fwmkeys", FwmkeysSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "fwmkeysAsync", FwmkeysAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "addint", AddintSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "addintAsync", AddintAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "adddouble", AdddoubleSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "adddoubleAsync", AdddoubleAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "sync", SyncSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "syncAsync", SyncAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "optimize", OptimizeSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "optimizeAsync", OptimizeAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "vanish", VanishSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "vanishAsync", VanishAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "copy", CopySync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "copyAsync", CopyAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "tranbegin", TranbeginSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "tranbeginAsync", TranbeginAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "trancommit", TrancommitSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "trancommitAsync", TrancommitAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "tranabort", TranabortSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "tranabortAsync", TranabortAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "path", PathSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "rnum", RnumSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "fsiz", FsizSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "setindex", SetindexSync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "setindexAsync", SetindexAsync);
    NODE_SET_PROTOTYPE_METHOD(Tmpl, "genuid", Genuid);

    target->Set(String::New("TDB"), Tmpl->GetFunction());
  }

private:

  static Handle<Value>
  New (const Arguments& args) {
    HandleScope scope;
    if (!args.IsConstructCall()) return args.Callee()->NewInstance();
    (new TDB)->Wrap(THIS);
    return THIS;
  }

  int Ecode () {
    return tctdbecode(tdb);
  }

  DEFINE_SYNC2(Ecode);

  const char * Errmsg (int ecode) {
    return tctdberrmsg(ecode);
  }

  DEFINE_SYNC2(Errmsg);

  bool Setmutex () {
    return tctdbsetmutex(tdb);
  }

  DEFINE_SYNC(Setmutex);

  bool Tune (int64_t bnum, int8_t apow, int8_t fpow, uint8_t opts) {
    return tctdbtune(tdb, bnum, apow, fpow, opts);
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
      opts = NOU(args[3]) ? 0 : args[3]->Int32Value();
    }

    bool run () {
      return tcw->Tune(bnum, apow, fpow, opts);
    }
  };

  DEFINE_SYNC(Tune)

  bool Setcache (int32_t rcnum, int32_t lcnum, int32_t ncnum) {
    return tctdbsetcache(tdb, rcnum, lcnum, ncnum);
  }

  class SetcacheData : public ArgsData {
  private:
    int32_t rcnum;
    int32_t lcnum;
    int32_t ncnum;

  public:
    SetcacheData (const Arguments& args) : ArgsData(args) {
      rcnum = NOU(args[0]) ? -1 : args[0]->Int32Value();
      lcnum = NOU(args[1]) ? -1 : args[0]->Int32Value();
      ncnum = NOU(args[2]) ? -1 : args[0]->Int32Value();
    }

    static bool
    checkArgs (const Arguments& args) {
      return (NOU(args[0]) || args[0]->IsNumber()) &&
        (NOU(args[1]) || args[1]->IsNumber()) &&
        (NOU(args[2]) || args[2]->IsNumber());
    }

    bool
    run () {
      return tcw->Setcache(rcnum, lcnum, ncnum);
    }
  };

  DEFINE_SYNC(Setcache);

  bool Setxmsiz (int64_t xmsiz) {
    return tctdbsetxmsiz(tdb, xmsiz);
  }

  DEFINE_SYNC(Setxmsiz);

  bool Setdfunit (int32_t dfunit) {
    return tctdbsetdfunit(tdb, dfunit);
  }

  DEFINE_SYNC(Setdfunit);

  bool Open (char *path, int omode) {
    return tctdbopen(tdb, path, omode);
  }

  class OpenData : public FilenameData {
  protected:
    int omode;

  public:
    OpenData (const Arguments& args) : FilenameData(args), ArgsData(args) {
      omode = NOU(args[1]) ? TDBOREADER : args[1]->Int32Value();
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
    return tctdbclose(tdb);
  }

  DEFINE_SYNC(Close);
  DEFINE_ASYNC(Close);

  bool Put(char *kbuf, int ksiz, TCMAP *map) {
    return tctdbput(tdb, kbuf, ksiz, map);
  }

  class PutData : public KeyData {
  protected:
    TCMAP *map;

  public:
    PutData (const Arguments& args) : KeyData(args), ArgsData(args) {
      map = objtotcmap(Local<Object>::Cast(args[1]));
    }

    ~PutData () {
      tcmapdel(map);
    }

    static bool checkArgs (const Arguments& args) {
      return args[1]->IsObject();
    }

    bool run () {
      return tcw->Put(*kbuf, ksiz, map);
    }
  };

  DEFINE_SYNC(Put);

  class PutAsyncData : public PutData, public AsyncData {
  public:
    PutAsyncData (const Arguments& args)
    : PutData(args), AsyncData(args[2]), ArgsData(args) {}
  };

  DEFINE_ASYNC(Put);

  bool Putkeep(char *kbuf, int ksiz, TCMAP *map) {
    return tctdbputkeep(tdb, kbuf, ksiz, map);
  }

  class PutkeepData : public PutData {
  public:
    PutkeepData (const Arguments& args) : PutData(args), ArgsData(args) {}

    bool run () {
      return tcw->Putkeep(*kbuf, ksiz, map);
    }
  };

  DEFINE_SYNC(Putkeep);

  class PutkeepAsyncData : public PutkeepData, public AsyncData {
  public:
    PutkeepAsyncData (const Arguments& args)
    : PutkeepData(args), AsyncData(args[2]), ArgsData(args) {}
  };

  DEFINE_ASYNC(Putkeep);

  bool Putcat(char *kbuf, int ksiz, TCMAP *map) {
    return tctdbputcat(tdb, kbuf, ksiz, map);
  }

  class PutcatData : public PutData {
  public:
    PutcatData (const Arguments& args) : PutData(args), ArgsData(args) {}

    bool run () {
      return tcw->Putcat(*kbuf, ksiz, map);
    }
  };

  DEFINE_SYNC(Putcat);

  class PutcatAsyncData : public PutcatData, public AsyncData {
  public:
    PutcatAsyncData (const Arguments& args)
    : PutcatData(args), AsyncData(args[2]), ArgsData(args) {}
  };

  DEFINE_ASYNC(Putcat);

  bool Out(char *kbuf, int ksiz) {
    return tctdbout(tdb, kbuf, ksiz);
  }

  DEFINE_SYNC(Out);
  DEFINE_ASYNC(Out);

  TCMAP * Get(char *kbuf, int ksiz) {
    return tctdbget(tdb, kbuf, ksiz);
  }

  class GetData : public KeyData {
  protected:
    TCMAP *map;

  public:
    GetData (const Arguments& args) : KeyData(args), ArgsData(args) {}

    ~GetData () {
      tcmapdel(map);
    }

    bool run () {
      map = tcw->Get(*kbuf, ksiz);
      return map != NULL;
    }

    Handle<Value>
    returnValue () {
      HandleScope scope;
      return scope.Close(tcmaptoobj(map));
    }
  };

  DEFINE_SYNC2(Get);

  class GetAsyncData : public GetData, public AsyncData {
  public:
    GetAsyncData (const Arguments& args)
    : GetData(args), AsyncData(args[1]), ArgsData(args) {}
  };

  DEFINE_ASYNC2(Get);

  int Vsiz(char *kbuf, int ksiz) {
    return tctdbvsiz(tdb, kbuf, ksiz);
  }

  DEFINE_SYNC2(Vsiz);
  DEFINE_ASYNC2(Vsiz);

  bool Iterinit () {
    return tctdbiterinit(tdb);
  }

  DEFINE_SYNC(Iterinit);
  DEFINE_ASYNC(Iterinit);

  char * Iternext (int *vsiz_p) {
    return static_cast<char *>(tctdbiternext(tdb, vsiz_p));
  }

  DEFINE_SYNC2(Iternext);
  DEFINE_ASYNC2(Iternext);

  TCLIST * Fwmkeys(char *kbuf, int ksiz, int max) {
    return tctdbfwmkeys(tdb, kbuf, ksiz, max);
  }

  DEFINE_SYNC2(Fwmkeys);
  DEFINE_ASYNC2(Fwmkeys);

  int Addint(char *kbuf, int ksiz, int num) {
    return tctdbaddint(tdb, kbuf, ksiz, num);
  }

  DEFINE_SYNC2(Addint);
  DEFINE_ASYNC2(Addint);

  double Adddouble(char *kbuf, int ksiz, double num) {
    return tctdbadddouble(tdb, kbuf, ksiz, num);
  }

  DEFINE_SYNC2(Adddouble);
  DEFINE_ASYNC2(Adddouble);

  bool Sync () {
    return tctdbsync(tdb);
  }

  DEFINE_SYNC(Sync);
  DEFINE_ASYNC(Sync);

  bool Optimize (int64_t bnum, int8_t apow, int8_t fpow, uint8_t opts) {
    return tctdboptimize(tdb, bnum, apow, fpow, opts);
  }

  class OptimizeData : public TuneData {
  public:
    OptimizeData (const Arguments& args) : TuneData(args) {}

    bool run () {
      return tcw->Optimize(bnum, apow, fpow, opts);
    }
  };

  DEFINE_SYNC(Optimize);

  class OptimizeAsyncData : public OptimizeData, public AsyncData {
  public:
    OptimizeAsyncData (const Arguments& args)
    : OptimizeData(args), AsyncData(args[4]) {}
  };

  DEFINE_ASYNC(Optimize);

  bool Vanish () {
    return tctdbvanish(tdb);
  }

  DEFINE_SYNC(Vanish);
  DEFINE_ASYNC(Vanish);

  bool Copy (char *path) {
    return tctdbcopy(tdb, path);
  }

  DEFINE_SYNC(Copy);
  DEFINE_ASYNC(Copy);

  bool Tranbegin () {
    return tctdbtranbegin(tdb);
  }

  DEFINE_SYNC(Tranbegin);
  DEFINE_ASYNC(Tranbegin);

  bool Trancommit () {
    return tctdbtrancommit(tdb);
  }

  DEFINE_SYNC(Trancommit);
  DEFINE_ASYNC(Trancommit);

  bool Tranabort () {
    return tctdbtranabort(tdb);
  }

  DEFINE_SYNC(Tranabort);
  DEFINE_ASYNC(Tranabort);

  const char * Path () {
    return tctdbpath(tdb);
  }

  DEFINE_SYNC2(Path);

  uint64_t Rnum () {
    return tctdbrnum(tdb);
  }

  DEFINE_SYNC2(Rnum)

  uint64_t Fsiz () {
    return tctdbfsiz(tdb);
  }

  DEFINE_SYNC2(Fsiz);

  bool Setindex (const char* name, int type) {
    return tctdbsetindex(tdb, name, type);
  }

  class SetindexData : public virtual ArgsData {
  private:
    String::Utf8Value name;
    int type;

  public:
    SetindexData (const Arguments& args) : name(args[0]), ArgsData(args) {
      type = args[1]->Int32Value();
    }

    static bool checkArgs (const Arguments& args) {
      return args[0]->IsString() && args[1]->IsNumber();
    }

    bool run () {
      return tcw->Setindex(*name, type);
    }
  };

  DEFINE_SYNC(Setindex);

  class SetindexAsyncData : public SetindexData, public AsyncData {
  public:
    SetindexAsyncData (const Arguments& args)
    : AsyncData(args[2]), SetindexData(args), ArgsData(args) {}
  };

  DEFINE_ASYNC(Setindex);

  // bug: JavaScript can't handle integers greater than Math.pow(2,53)
  static Handle<Value>
  Genuid (const Arguments& args) {
    HandleScope scope;
    int64_t siz = tctdbgenuid(ObjectWrap::Unwrap<TDB>(THIS)->tdb);
    return Integer::New(siz);
  }
};

const Persistent<FunctionTemplate> TDB::Tmpl =
  Persistent<FunctionTemplate>::New(FunctionTemplate::New(TDB::New));

class QRY : TCWrap {
public:
  QRY (TCTDB *db) {
    qry = tctdbqrynew(db);
  }

  ~QRY () {
    tctdbqrydel(qry);
  }

  static QRY *
  Unwrap (const Handle<Object> obj) {
    return ObjectWrap::Unwrap<QRY>(obj);
  }

  static TDBQRY *
  Backend (const Handle<Object> obj) {
    return Unwrap(obj)->qry;
  }

  static void
  Initialize (const Handle<Object> target) {
    HandleScope scope;
    Local<FunctionTemplate> tmpl = FunctionTemplate::New(New);
    set_ecodes(tmpl);

    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCSTREQ);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCSTRINC);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCSTRBW);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCSTREW);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCSTRAND);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCSTROR);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCSTROREQ);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCSTRRX);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCNUMEQ);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCNUMGT);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCNUMGE);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCNUMLT);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCNUMLE);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCNUMBT);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCNUMOREQ);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCFTSPH);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCFTSAND);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCFTSOR);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCFTSEX);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCNEGATE);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QCNOIDX);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QOSTRASC);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QOSTRDESC);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QONUMASC);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QONUMDESC);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QPPUT);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QPOUT);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, QPSTOP);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, MSUNION);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, MSISECT);
    DEFINE_PREFIXED_CONSTANT(tmpl, TDB, MSDIFF);
      
    NODE_SET_PROTOTYPE_METHOD(tmpl, "addcond", Addcond);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "setorder", Setorder);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "setlimit", Setlimit);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "search", SearchSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "searchAsync", SearchAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "searchout", SearchoutSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "searchoutAsync", SearchoutAsync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "hint", Hint);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "metasearch", MetasearchSync);
    NODE_SET_PROTOTYPE_METHOD(tmpl, "metasearchAsync", MetasearchAsync);

    Local<ObjectTemplate> ot = tmpl->InstanceTemplate();
    ot->SetInternalFieldCount(1);

    target->Set(String::New("TDBQRY"), tmpl->GetFunction());
  }

private:
  TDBQRY *qry;

  static Handle<Value>
  New (const Arguments& args) {
    HandleScope scope;
    if (args.Length() < 1 ||
        !TDB::Tmpl->HasInstance(args[0])) {
      return THROW_BAD_ARGS;
    }
    TCTDB *db = ObjectWrap::Unwrap<TDB>(Local<Object>::Cast(args[0]))->tdb;
    (new QRY(db))->Wrap(THIS);
    return THIS;
  }

  static Handle<Value>
  Addcond (const Arguments& args) {
    HandleScope scope;
    if (!args[1]->IsNumber()) {
      return THROW_BAD_ARGS;
    }
    tctdbqryaddcond(
                    Backend(THIS),
                    *String::Utf8Value(args[0]),
                    args[1]->Int32Value(),
                    *String::Utf8Value(args[2]));
    return Undefined();
  }

  static Handle<Value>
  Setorder (const Arguments& args) {
    HandleScope scope;
    if (!(args[1]->IsNumber() || NOU(args[1]))) {
      return THROW_BAD_ARGS;
    }
    tctdbqrysetorder(
                     Backend(THIS),
                     *String::Utf8Value(args[0]),
                     NOU(args[1]) ? TDBQOSTRASC : args[1]->Int32Value());
    return Undefined();
  }

  static Handle<Value>
  Setlimit (const Arguments& args) {
    HandleScope scope;
    if (!(args[0]->IsNumber() || NOU(args[0])) ||
        !(args[1]->IsNumber() || NOU(args[1]))) {
      return THROW_BAD_ARGS;
    }
    tctdbqrysetlimit(
                     Backend(THIS),
                     NOU(args[1]) ? -1 : args[0]->Int32Value(),
                     NOU(args[1]) ? -1 : args[1]->Int32Value());
    return Undefined();
  }

  TCLIST * Search () {
    return tctdbqrysearch(qry);
  }

  class SearchData : public virtual ArgsData {
  private:
    TCLIST *list;

  public:
    SearchData (const Arguments& args) : ArgsData(args) {}

    ~SearchData () {
      tclistdel(list);
    }

    bool run () {
      list = tcw->Search();
      return true;
    }

    Handle<Value> returnValue () {
      HandleScope scope;
      return scope.Close(tclisttoary(list));
    }
  };

  DEFINE_SYNC2(Search);

  class SearchAsyncData : public SearchData, public AsyncData {
  public:
    SearchAsyncData (const Arguments& args)
    : SearchData(args), AsyncData(args[0]), ArgsData(args) {}
  };

  DEFINE_ASYNC2(Search);

  bool Searchout () {
    return tctdbqrysearchout(qry);
  }

  class SearchoutData : public virtual ArgsData {
  public:
    SearchoutData (const Arguments& args) : ArgsData(args) {}

    bool run () {
      return tcw->Searchout();
    }
  };

  DEFINE_SYNC(Searchout);

  class SearchoutAsyncData : public SearchoutData, public AsyncData {
  public:
    SearchoutAsyncData (const Arguments& args)
    : SearchoutData(args), AsyncData(args[0]), ArgsData(args) {}
  };

  DEFINE_ASYNC(Searchout);

  static Handle<Value>
  Hint (const Arguments& args) {
    HandleScope scope;
    const char *hint = tctdbqryhint(Backend(THIS));
    return String::New(hint);
  }

  TCLIST * Metasearch (TDBQRY **qrys, int num, int type) {
    return tctdbmetasearch(qrys, num, type);
  }

  class MetasearchData : public virtual ArgsData {
  private:
    TDBQRY **qrys;
    int qnum;
    int type;
    TCLIST *list;

  public:
    MetasearchData (const Arguments& args) : ArgsData(args) {
      Local<Array> others = Local<Array>::Cast(args[0]);
      int num = others->Length();
      qrys = static_cast<TDBQRY **>(tcmalloc(sizeof(*qrys) * (num+1)));
      qnum = 0;
      qrys[qnum++] = Backend(THIS);
      Local<Object> oqry;
      for (int i = 0; i < num; i++) {
        oqry = others->CloneElementAt(i);
        if (TDB::Tmpl->HasInstance(oqry)) {
          qrys[qnum++] = Backend(oqry);
        }
      }
      type = NOU(args[1]) ? TDBMSUNION : args[1]->Int32Value();
    }

    ~MetasearchData () {
      tcfree(qrys);
      tclistdel(list);
    }

    bool run () {
      list = tcw->Metasearch(qrys, qnum, type);
      return true;
    }

    static bool checkArgs (const Arguments& args) {
      return args[0]->IsArray() &&
        (args[1]->IsNumber() || NOU(args[1]));
    }

    Handle<Value> returnValue () {
      HandleScope scope;
      return scope.Close(tclisttoary(list));
    }
  };

  DEFINE_SYNC2(Metasearch);

  class MetasearchAsyncData : public MetasearchData, public AsyncData {
  public:
    MetasearchAsyncData (const Arguments& args)
    : MetasearchData(args), AsyncData(args[2]), ArgsData(args) {}
  };

  DEFINE_ASYNC2(Metasearch);
};

MODULE_INIT(tdb, CLASS_INIT(TDB) CLASS_INIT(QRY));
