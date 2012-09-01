// Hash db bench

var sys = require('util');
var tc = require('../tc');
var fs = require('fs');

sys.puts("Tokyo Cabinet version " + tc.ver);

var samples = [];
var next_sample = function () {
  var next = samples.shift();
  if (next) next();
};
setTimeout(next_sample, 10);

var put_count = 100000;

var hdb;

samples.push(function() {
  sys.puts('sync');

  hdb = tc.hdb();
  try {
    hdb.openSync('casket1.tch', 'wc');
  } catch(e) {
    sys.error(e);
  }

  var i, t = Date.now();
  for (i = 0; i < put_count; i++) {
    try {
      hdb.putSync('key' + i, 'val' + i + ' 0123456789');
    } catch(e) {
      sys.error(e);
    }
  }
  sys.puts(Date.now() - t);
  for (i = 0; i < 10; i++) {
    try {
      sys.puts(hdb.getSync('key' + i));
    } catch(e) {
      sys.error(e);
    }
  }

  next_sample();
});

samples.push(function() {
  sys.puts('async');

  var hdb = tc.hdb();

  // this line is necessary for an async operation
  hdb.setmutex();

  hdb.open('casket2.tch', 'wc', function(e) {
    if (e) sys.error(hdb.errmsg(e));

    var t = Date.now();
    var n = put_count;
    for (var i = 0; i < put_count; i++) {
      hdb.put('key' + i, 'val' + i + ' 0123456789', function(e) {
        if (e) sys.error(e);

        if (--n === 0) {
          sys.puts(Date.now() - t);

          n = 10;
          for (var i = 0; i < 10; i++) {
            hdb.get('key' + i, function(e, val) {
              if (e) {
                sys.puts(e);
              } else {
                sys.puts(val);
              }
              if (--n === 0) {
                next_sample();
              }
            });
          }
        }
      });
    }
  });
});

samples.push(function() {
  sys.puts('async');

  var hdb = tc.hdb();
  // this line is necessary for an async operation
  hdb.setmutex();

  hdb.open('casket3.tch', 'wc', function(e) {
    if (e) sys.error(e);

    var t = Date.now();
    var i = 0;
    (function func() {
      hdb.put('key' + i, 'val' + i + ' 0123456789', function(e) {
        if (e) sys.error(e);

        if (++i < put_count) {
          func();
        } else {
          sys.puts(Date.now() - t);

          var n = 10;
          for (var j = 0; j < 10; j++) {
            hdb.get('key' + j, function(e, val) {
              if (e) {
                sys.puts(e);
              } else {
                sys.puts(val);
              }
              if (--n === 0) {
                next_sample();
              }
            });
          }
        }
      });
    }());
  });
});
