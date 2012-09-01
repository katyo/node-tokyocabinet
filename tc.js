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

function TCError(self, func, args){
  Error.call(this);
  Error.captureStackTrace(this, arguments.callee);
  this.name = 'TCError';
  if(isFunc(self.errmsg) && isFunc(self.ecode)){
    this.message = self.errmsg(self.ecode());
  }
  if(func){
    this.message += '\n   call: ' + func + '(' + FArgs(args) + ')';
  }
};
TCError.prototype.__proto__ = Error.prototype;

var hooks = {
  open: function(type, clas, orig){
    return function(){
      if(isStr(arguments[1])){
        var i, m,
        mode = 0,
        opts = arguments[1].split(/[\|\+\.\-\/]/);
        for(i = 0; i < opts.length; i++){
          m = ('o' + opts[i]).toUpperCase();
          if(m in clas){
            mode |= clas[m];
          }
        }
        arguments[1] = mode;
      }/*else if(type == 'async' && (isFunc(arguments[1]) || )){
        arguments.push(arguments[1]);
        arguments[1] = clas.OREADER | clas.OCREAT;
      }*/
      return orig.apply(this, arguments);
    };
  },
  addcond: function(type, clas, orig){
    var cond = {
      /* string */
      '~=': clas.QCSTREQ,   /* equal */
      '~@': clas.QCSTRINC,  /* include */
      '@':  clas.QCSTRINC,
      '~^': clas.QCSTRBW,   /* begin */
      '^':  clas.QCSTRBW,
      '~$': clas.QCSTREW,   /* end */
      '$':  clas.QCSTREW,
      '~&': clas.QCSTRAND,  /* all */
      '&':  clas.QCSTRAND,
      '~|': clas.QCSTROR,   /* one */
      '|':  clas.QCSTROR,
      '~*': clas.QCSTROREQ, /* in (tokens) */
      '~?': clas.QCSTRRX,   /* regexp */
      '~':  clas.QCSTRRX,
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
      '=': function(arg){ return isStr(arg) ? '~=' : '=='; },
      '*': function(arg){ return isStr(arg) ? '~*' : '=*'; }
    };
    return clas == TC.TDBQRY ? function(){
      if(isStr(arguments[1])){
        var a = arguments[1],
        c = 0, i, t;
        for(; a; ){
          for(i in cond){
            a = a.split(i);
            if(a.length > 1){
              t = cond[i];
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
      '<':  clas.QONUMDESC
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
OK[TC.ENOREC] = 0;
OK[TC.EKEEP] = 0;

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
                if(arguments[0]){
                  arguments[0] = new TCError(self, pure, args);
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
