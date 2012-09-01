var TC = require('./tc_all'),
UT = require('util');

var reSync = /Sync$/,
isSync = function(name){
  return reSync.test(name);
},
reAsync = /Async$/,
isAsync = function(name){
  return reAsync.test(name);
},
pureName = function(name){
  return name.replace(reAsync, '').replace(reSync, '');
},
isStr = function(data){
  return typeof data == 'string';
},
isNum = function(data){
  return typeof data == 'number';
},
isFunc = function(data){
  return typeof data == 'function';
};

function FArgs(args){
  if(args && args.length){
    var i, a, r = [];
    for(i = 0; i < args.length; i++){
      a = args[i];
      switch(typeof a){
      case 'function':
        a = '[Function]';
        break;
      case 'string':
        a = '"' + a + '"';
        break;
      default:
        a = UT.format(a);
      }
      r.push(a);
    }
    return r.join(', ');
  }
  return '';
}

function TCError(self, func, args, ecode){
  Error.call(this);
  Error.captureStackTrace(this, arguments.callee);
  this.name = 'TCError';
  this.code = typeof ecode == 'number' ? ecode : isFunc(self.ecode) ? self.ecode() : 0;
  if(this.code && isFunc(self.errmsg)){
    this.message = self.errmsg(this.code);
  }
  if(func){
    this.message += '\n   call: ' + func + '(' + FArgs(args) + ')';
  }
};
TCError.prototype.__proto__ = Error.prototype;

var hooks = {
  open: function(type, clas, orig){
    var modes = {
      'reader': clas.OREADER,
      'writer': clas.OWRITER,
      'create': clas.OCREAT,
      'creat':  clas.OCREAT,
      'trunc':  clas.OTRUNC,
      'tsync':  clas.OTSYNC,
      'sync':   clas.OTSYNC,
      'nolock': clas.ONOLCK,
      'nolck':  clas.ONOLCK,
      'locknb': clas.OLCKNB,
      'lcknb':  clas.OLCKNB,

      /* short */
      'r':      clas.OREADER,
      'w':      clas.OWRITER,
      'c':      clas.OCREAT,
      't':      clas.OTRUNC,
      's':      clas.OTSYNC,
      'nl':     clas.ONOLCK,
      'nb':     clas.OLCKNB,

      /* skip */
      ' ':      0,
      '+':      0,
      '|':      0,
      '/':      0,
      ',':      0
    };
    return function(){
      if(isStr(arguments[1])){
        var a = arguments[1],
        m = 0, i, t;
        for(; a; ){
          for(i in modes){
            a = a.split(i);
            if(a.length > 1){
              t = modes[i];
              if(isFunc(t)){
                t = t(arguments[2]);
              }
              m |= t;
            }
            a = a.join('');
          }
        }
        arguments[1] = m;
      }/*else if(type == 'async' && (isFunc(arguments[1]) || )){
        arguments.push(arguments[1]);
        arguments[1] = clas.OREADER | clas.OCREAT;
      }*/
      return orig.apply(this, arguments);
    };
  },
  addcond: function(type, clas, orig){
    var conds = {
      /* string */
      '~=': clas.QCSTREQ,   /* equal */
      '~@': clas.QCSTRINC,  /* include */
      '~^': clas.QCSTRBW,   /* begin */
      '~$': clas.QCSTREW,   /* end */
      '~&': clas.QCSTRAND,  /* all */
      '~|': clas.QCSTROR,   /* one */
      '~*': clas.QCSTROREQ, /* in (tokens) */
      '~?': clas.QCSTRRX,   /* regexp */

      /* number */
      '==': clas.QCNUMEQ,
      '>':  clas.QCNUMGT,
      '>=': clas.QCNUMGE,
      '<':  clas.QCNUMLT,
      '<=': clas.QCNUMLT,
      '<>': clas.QCNUMBT,   /* between */
      '><': clas.QCNUMBT,
      '=*': clas.QCNUMOREQ, /* in (tokens) */

      /* fulltext */
      '?~': clas.QCFTSPH,   /* phrase */
      '?&': clas.QCFTSAND,  /* all */
      '?|': clas.QCFTSOR,   /* one */
      '?*': clas.QCFTSEX,   /* compound */

      /* flags */
      '!': clas.QCNEGATE,   /* negate */
      '%': clas.QCNOIDX,    /* no index */

      /* short */
      '=': function(arg){ return isStr(arg) ? '~=' : '=='; },
      '*': function(arg){ return isStr(arg) ? '~*' : '=*'; },

      /* string */
      '@':  clas.QCSTRINC,  /* include */
      '^':  clas.QCSTRBW,   /* begin */
      '$':  clas.QCSTREW,   /* end */
      '&':  clas.QCSTRAND,  /* all */
      '|':  clas.QCSTROR,   /* one */
      '~':  clas.QCSTRRX,   /* regexp */

      /* skip */
      ' ':      0,
      '/':      0,
      ',':      0
    };
    return clas == TC.TDBQRY ? function(){
      if(isStr(arguments[1])){
        var a = arguments[1],
        c = 0, i, t;
        for(; a; ){
          for(i in conds){
            a = a.split(i);
            if(a.length > 1){
              t = conds[i];
              if(isFunc(t)){
                t = t(arguments[2]);
              }
              c |= t;
            }
            a = a.join('');
          }
        }
        arguments[1] = c;
      }
      orig.apply(this, arguments);
      return this;
    } : orig;
  },
  setorder: function(type, clas, orig){
    var ord = {
      '~>': clas.QOSTRASC,
      '~<': clas.QOSTRDESC,
      '>':  clas.QONUMASC,
      '<':  clas.QONUMDESC,

      '~v':  clas.QOSTRASC,
      '~^':  clas.QOSTRDESC,
      'v':  clas.QONUMASC,
      '^':  clas.QONUMDESC
    };
    return clas == TC.TDBQRY ? function(){
      if(isStr(arguments[1])){
        var a = arguments[1],
        o = 0, i;
        for(; a; ){
          for(i in ord){
            if(a.search(i) > -1){
              a = a.split(i).join('');
              o |= ord[i];
            }
          }
        }
        arguments[1] = o;
      }
      orig.apply(this, arguments);
      return this;
    } : orig;
  },
  setlimit: function(type, clas, orig){
    return clas == TC.TDBQRY ? function(){
      orig.apply(this, arguments);
      return this;
    } : orig;
  }
};

function Hook(name, type, clas, orig){
  if(isFunc(hooks[name])){
    return hooks[name](type, clas, orig);
  }
  return orig;
}

var OK = {};
OK[TC.ESUCCESS] = 0;
OK[TC.ENOREC] = 'norec';
OK[TC.EKEEP] = 'keep';

function Class(Native){
  var n,
  Proto = Native.prototype;

  for(n in Proto){
    (function(name, data, pure){
      if(isFunc(data)){
        if(isSync(name)){
          Proto[name] = Hook(pure, 'sync', Native, function(){
            var ret = data.apply(this, arguments);
            if(isFunc(this.ecode) && !(this.ecode() in OK)){
              throw new TCError(this, name, arguments);
            }
            return ret;
          });
        }else if(isAsync(name)){
          delete Proto[name];
          Proto[pure] = Hook(pure, 'async', Native, function(){
            if(arguments.length > 0 && isFunc(arguments[arguments.length - 1])){
              var callback = arguments[arguments.length - 1],
              args = arguments,
              self = this;
              arguments[arguments.length - 1] = function(){
                var code = arguments[0];
                if(code){
                  if(code in OK){
                    code = OK[code];
                    if(code){
                      Array.prototype.push.call(arguments, code);
                    }
                    arguments[0] = null;
                  }else{
                    arguments[0] = new TCError(self, pure, args, code);
                  }
                }
                callback.apply(this, arguments);
              };
            }
            data.apply(this, arguments);
          });
        }else{
          Proto[pure] = Hook(pure, '', Native, data);
        }
      }
    })(n, Proto[n], pureName(n));
  }
  return Native;
}

function Alias(Native, tab){
  var s, d, Proto = Native.prototype;
  for(d in tab){
    s = tab[d];
    if(s in Proto){
      Proto[d] = Proto[s];
    }
    if(s in Native){
      Native[d] = Native[s];
    }
  }
  return Native;
}

exports = module.exports = {
  ver: TC.VERSION,
  hdb: Class(TC.HDB),
  bdb: Class(TC.BDB),
  fdb: Class(TC.FDB),
  tdb: Class(TC.TDB),
  adb: Class(TC.ADB)
};

exports.bdb.cur = Class(TC.BDBCUR);

exports.tdb.qry = Alias(Class(TC.TDBQRY), {
  cond: 'addcond',
  order: 'setorder',
  limit: 'setlimit'
});
