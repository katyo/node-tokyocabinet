// sample codes at http://1978th.net/tokyocabinet/spex-en.html
// translated into JS.

var sys = require('util');
var tc = require('../tc');
var fs = require('fs');

sys.puts("Tokyo Cabinet version " + tc.ver);

var samples = [];
var next_sample = function () {
  var next = samples.shift();
  if (next) next();
}
setTimeout(next_sample, 10);

samples.push(function() {
  sys.puts("== Sample: HDB ==");

  var hdb = new tc.hdb();
  // this line is necessary for an async operation
  hdb.setmutex();

  hdb.open('casket.tch', 'wc', function(e) {
    if (e) sys.error(e);

    var n = 3;
    [["foo", "hop"], ["bar", "step"], ["baz", "jump"]].forEach(function(kv) {
      hdb.put(kv[0], kv[1], function(e) {
        if (e) sys.error(hdb.errmsg(e));

        if (--n === 0) {
          hdb.get("foo", function(e, value) {
            if (e) sys.error(e);

            sys.puts(value);

            hdb.iterinit(function(e) {
              if (e) sys.error(e);

              hdb.iternext(function func(e, key, res) { // recursive asynchronous function
                if (e) sys.error(e);

                if (res !== 'norec') { // if next key exsists

                  hdb.get(key, function(e, value) {
                    if (e) sys.error(e);

                    sys.puts(key + ':' + value);

                    hdb.iternext(func);
                  });

                } else { // if next key does not exist

                  hdb.close(function(e) {
                    if (e) sys.error(e);

                    fs.unlink('casket.tch');

                    next_sample();
                  });

                }
              });
            });
          });
        }
      });
    });
  });

});

samples.push(function() {
  sys.puts("== Sample: BDB ==");

  var bdb = new tc.bdb();
  // this line is necessary for an async operation
  if (!bdb.setmutex()) throw bdb.errmsg();

  bdb.open('casket.tcb', 'wc', function(e) {
    if (e) sys.error(bdb.errmsg(e));

    var n = 3;
    [["foo", "hop"], ["bar", "step"], ["baz", "jump"]].forEach(function(kv) {
      bdb.put(kv[0], kv[1], function(e) {
        if (e) sys.error(e);

        if (--n === 0) {
          bdb.get("foo", function(e, value) {
            sys.puts(value);

            var cur = bdb.cur(bdb);
            cur.first(function func(e) { // recursive asynchronous function
              if (e) sys.error(e);

              cur.key(function(e, key, rec) {
                if (e) sys.error(e);

                if (rec != 'norec') {

                  cur.val(function(e, val) {
                    if (e) sys.error(e);

                    sys.puts(key + ':' + val);

                    cur.next(func);
                  });

                } else { // if next key does not exist

                  bdb.close(function(e) {
                    if (e) sys.error(e);

                    fs.unlink('casket.tcb');

                    next_sample();
                  });

                }
              });
            });
          });
        }
      });
    });
  });

});

samples.push(function() {
  sys.puts("== Sample: FDB ==");

  var fdb = new tc.fdb();
  // this line is necessary for an async operation
  fdb.setmutex();

  fdb.open('casket.tcf', 'wc', function(e) {
    if (e) sys.error(e);

    var n = 3;
    [["1", "one"], ["12", "twelve"], ["144", "one forty four"]].forEach(function(kv) {
      fdb.put(kv[0], kv[1], function(e) {
        if (e) sys.error(e);

        if (--n === 0) {
          fdb.get("1", function(e, value) {
            if (e) sys.error(e);
            sys.puts(value);

            fdb.iterinit(function(e) {
              if (e) sys.error(e);

              fdb.iternext(function func(e, key, rec) { // recursive asynchronous function
                if (e) sys.error(e);

                if (rec != 'norec') { // if next key exsists

                  fdb.get(key, function(e, value) {
                    if (e) sys.error(e);

                    sys.puts(key + ':' + value);

                    fdb.iternext(func);
                  });

                } else { // if next key does not exist

                  fdb.close(function(e) {
                    if (e) sys.error(e);

                    fs.unlink('casket.tcf');

                    next_sample();
                  });

                }
              });
            });
          });
        }
      });
    });
  });

});

samples.push(function() {
  sys.puts("== Sample: TDB ==");

  var tdb = new tc.tdb;
  // this line is necessary for an async operation
  tdb.setmutex();

  tdb.open('casket.tct', 'wc', function(e) {
    if (e) sys.error(e);

    var n = 3;
    [[tdb.genuid(), {"name": "mikio", "age": "30", "lang": "ja,en,c"}],
    [tdb.genuid(), {"name": "joker", "age": "19", "lang": "en,es"}],
    [tdb.genuid(), {"name": "falcon", "age": "31","lang": "ja"}]].forEach(function(kv) {

      tdb.put(kv[0], kv[1], function(e) {
        if (e) sys.error(e);

        if (--n === 0) {
          var qry = tdb.qry(tdb)
            .cond("age", ">=", 20)
            .cond("lang", "|", "ja,en")
            .order("name", "~v")
            .limit(10, 0)
            .search(function(e, res) {
              if (e) sys.error(e);

              n = res.length;
              res.forEach(function(r) {
                tdb.get(r, function(e, cols) {
                  if (e) sys.error(e);

                  if (cols) {
                    sys.print(r);
                    Object.keys(cols).forEach(function(name) {
                      sys.print("\t" + name + "\t" + cols[name]);
                    });
                    sys.print("\n");
                  }

                  if (--n === 0) {
                    tdb.close(function(e) {
                      if (e) sys.error(e);

                      fs.unlink('casket.tct');

                      next_sample();
                    });
                  }
                });
              });
            });
        }
      });
    });
  });

});

samples.push(function() {
  sys.puts("== Sample: ADB ==");

  var adb = new tc.adb;

  // no need to manually set mutex since adb automatically does it on open
  adb.open('casket.tcb', function(e) {
    if (e) sys.error(e);

    var n = 3;
    [["foo", "hop"], ["bar", "step"], ["baz", "jump"]].forEach(function(kv) {
      adb.put(kv[0], kv[1], function(e) {
        if (e) sys.error([e, "put error"]);

        if (--n === 0) {
          adb.get("foo", function(e, value) {
            if (e) sys.error([e, 'get error']);
            sys.puts(value);

            adb.iterinit(function(e) {
              if (e) sys.error([e, 'iterinit error']);

              adb.iternext(function func(e, key) { // recursive asynchronous function
                if (key !== null) { // if next key exsists

                  adb.get(key, function(e, value) {
                    if (e) sys.error([e, 'get error']);

                    sys.puts(key + ':' + value);

                    adb.iternext(func);
                  });

                } else { // if next key does not exist

                  adb.close(function(e) {
                    if (e) sys.error([e, 'close error']);

                    fs.unlink('casket.tcb');

                    next_sample();
                  });

                }
              });
            });
          });
        }
      });
    });
  });

});
