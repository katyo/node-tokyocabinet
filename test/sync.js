// sample codes at http://1978th.net/tokyocabinet/spex-en.html
// translated into JS.

var sys = require('util');
var tc = require('../tc');
var fs = require('fs');

sys.puts("Tokyo Cabinet version " + tc.ver);

(function() {
  sys.puts("== Sample: HDB ==");

  var hdb = tc.hdb();

  try {
    hdb.openSync('casket.tch', 'wc');
  } catch(e) {
    sys.error(e);
  }

  try {
    hdb.putSync("foo", "hop");
    hdb.putSync("bar", "step");
    hdb.putSync("baz", "jump");
  } catch(e) {
    sys.error(e);
  }

  try {
    sys.puts(hdb.getSync("foo"));
  } catch(e) {
    sys.error(e);
  }

  try {
    hdb.iterinitSync()
    var key, value;
    while ((key = hdb.iternextSync()) !== null) {
      value = hdb.getSync(key);
      if (value !== null) {
        sys.puts(key + ':' + value);
      }
    }
  } catch(e) {
    sys.error(e);
  }

  try {
    hdb.closeSync();
  } catch(e) {
    sys.error(e);
  }

  fs.unlink('casket.tch');
}());

(function() {
  sys.puts("== Sample: BDB ==");

  var bdb = tc.bdb();

  try {
    bdb.openSync('casket.tcb', 'wc');
  } catch(e) {
    sys.error(e);
  }

  try {
    bdb.putSync("foo", "hop");
    bdb.putSync("bar", "step");
    bdb.putSync("baz", "jump");
  } catch(e) {
    sys.error(e);
  }

  try {
    sys.puts(bdb.getSync("foo"));
  } catch(e) {
    sys.error(e);
  }

  var cur = bdb.cur(bdb);

  try {
    cur.firstSync();
    var key, value;
    while ((key = cur.keySync()) !== null) {
      value = cur.valSync();
      if (value) {
        sys.puts(key + ":" + value);
      }
      cur.nextSync();
    }
  } catch(e) {
    sys.error(e);
  }

  if (!bdb.closeSync()) {
    sys.error(bdb.errmsg());
  }

  fs.unlink('casket.tcb');
}());

(function() {
  sys.puts("== Sample: FDB ==");

  var fdb = tc.fdb();

  try {
    fdb.openSync('casket.tcf', 'wc');
  } catch(e) {
    sys.error(e);
  }

  try {
    fdb.putSync("1", "one");
    fdb.putSync("12", "twelve");
    fdb.putSync("144", "one forty four");
  } catch(e) {
    sys.error(e);
  }

  try {
    sys.puts(fdb.getSync("1"));
  } catch(e) {
    sys.error(e);
  }

  try {
    fdb.iterinitSync();
    var key, value;
    while ((key = fdb.iternextSync()) != null) {
      value = fdb.getSync(key);
      if (value) {
        sys.puts(key + ':' + value);
      }
    }
  } catch(e) {
    sys.error(e);
  }

  try {
    fdb.closeSync();
  } catch(e) {
    sys.error(e);
  }

  fs.unlink('casket.tcf');
}());

(function() {
  sys.puts("== Sample: TDB ==");

  var tdb = tc.tdb();

  try {
    tdb.openSync('casket.tct', 'wc');
  } catch(e) {
    sys.error(e);
  }

  var pk = tdb.genuid();
  var cols = {"name": "mikio", "age": "30", "lang": "ja,en,c"};
  try {
    tdb.putSync(pk, cols);
  } catch(e) {
    sys.error(e);
  }

  var pk = tdb.genuid();
  var cols = {"name": "joker", "age": "19", "lang": "en,es"};
  try {
    tdb.putSync(pk, cols);
  } catch(e) {
      sys.error(e);
  }

  pk = "12345";
  cols = {};
  cols.name = "falcon";
  cols.age = "31";
  cols.lang = "ja";
  try {
    tdb.putSync(pk, cols);
  } catch(e) {
    sys.error(e);
  }

  var qry = tdb.qry(tdb)
    .cond("age", ">=", 20)
    .cond("lang", "|", "ja,en")
    .order("name", "~v")
    .limit(10, 0);
  var res = qry.searchSync();
  res.forEach(function(r) {
    var cols = tdb.getSync(r);
    if (cols) {
      sys.print(r);
      Object.keys(cols).forEach(function(name) {
        sys.print("\t" + name + "\t" + cols[name]);
      });
      sys.print("\n");
    }
  });

  try {
    tdb.closeSync();
  } catch(e) {
    sys.error(e);
  }

  fs.unlink('casket.tct');
}());

(function() {
  sys.puts("== Sample: ADB ==");

  var adb = tc.adb();

  try {
    adb.openSync('casket.tcb');
  } catch(e) {
    sys.error("open error");
  }

  try {
    adb.putSync("foo", "hop");
    adb.putSync("bar", "step");
    adb.putSync("baz", "jump");
  } catch(e) {
    sys.error("put error");
  }

  try {
    sys.puts(adb.getSync("foo"));
  } catch(e) {
    sys.error("get error");
  }

  try {
    adb.iterinitSync();
    var key, value;
    while((key = adb.iternextSync()) != null) {
      value = adb.getSync(key);
      if (value) {
        sys.puts(key + ':' + value);
      }
    }
  } catch(e) {
    sys.error("iter error");
  }

  try {
    adb.closeSync();
  } catch(e) {
    sys.error("close error");
  }

  fs.unlink('casket.tcb');
}());
