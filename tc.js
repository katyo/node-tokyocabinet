var TC = require('./tc_all'),
UT = require('util');

/* commons */
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
},
isObj = function(data){
  return typeof data == 'object';
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

function AStr2Bits(args, num, opts){
  var arg = args[num], opt, bit;
  if(isStr(arg)){
    args[num] = 0;
    for(; arg; ){
      for(opt in opts){
        arg = arg.split(opt);
        if(arg.length > 1){
          bit = opts[opt];
          if(isFunc(bit)){
            bit = bit(args);
          }
          args[num] |= bit;
        }
        arg = arg.join('');
      }
    }
  }
}

function forFunc(Proto, Cb){
  for(var n in Proto){
    if(isFunc(Proto[n])){
      Cb(n, Proto[n], pureName(n));
    }
  }
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

var alias = {
  /* common aliases */
  mutex:     'setmutex',
  cache:     'setcache',
  xmsiz:     'setxmsiz',
  dfunit:    'setdfunit',
  recs:      'rnum',
  size:      'fsiz',
  purge:     'vanish',
  /* for tdb */
  index:     'setindex',
  indexSync: 'setindexSync',
  uid:       'genuid',
  /* for tdbqry */
  qry: {
    cond:    'addcond',
    order:   'setorder',
    limit:   'setlimit'
  }
};

var optsArgNum = {
  hdb: 3,
  bdb: 5,
  tdb: 3
}, optsArgBit = {};

var hooks = {
  open: function(Class, Native, orig){
    if(Class != 'adb'){
      var modes = {
        reader: Native.OREADER,
        writer: Native.OWRITER,
        create: Native.OCREAT,
        creat:  Native.OCREAT,
        trunc:  Native.OTRUNC,
        tsync:  Native.OTSYNC,
        sync:   Native.OTSYNC,
        nolock: Native.ONOLCK,
        nolck:  Native.ONOLCK,
        locknb: Native.OLCKNB,
        lcknb:  Native.OLCKNB,
        
        /* short */
        r:      Native.OREADER,
        w:      Native.OWRITER,
        c:      Native.OCREAT,
        t:      Native.OTRUNC,
        s:      Native.OTSYNC,
        nl:     Native.ONOLCK,
        nb:     Native.OLCKNB,
        
        /* skip */
        ' ':      0,
        '+':      0,
        '|':      0,
        '/':      0,
        ',':      0
      };
      return function(){
        AStr2Bits(arguments, 1, modes);
        return orig.apply(this, arguments);
      };
    }
  },
  tune: function(Class, Native, orig){
    if(optsArgNum[Class]){
      var opts = optsArgBit[Class] = {
        'large':   Native.TLARGE,
        'deflate': Native.TDEFLATE,
        'bzip':    Native.TBZIP,
        'tcbs':    Native.TTCBS,
        
        /* short */
        'l':       Native.TLARGE,
        'd':       Native.TDEFLATE,
        'b':       Native.TBZIP,
        't':       Native.TTCBS,
        
        /* skip */
        ' ':       0,
        ',':       0,
        '+':       0
      }, argn = optsArgNum[Class];
      return function(){
        AStr2Bits(arguments, argn, opts);
        orig.apply(this, arguments);
        return this;
      };
    }
  },
  optimize: function(Class, Native, orig){
    if(optsArgNum[Class]){
      var opts = optsArgBit[Class],
      argn = optsArgNum[Class];
      return function(){
        AStr2Bits(arguments, argn, opts);
        orig.apply(this, arguments);
        return this;
      };
    }
  },
  setcache: function(Class, Native, orig){
    return function(){
      orig.apply(this, arguments);
      return this;
    };
  },
  setxmsiz: function(Class, Native, orig){
    return function(){
      orig.apply(this, arguments);
      return this;
    };
  },
  setdfunit: function(Class, Native, orig){
    return function(){
      orig.apply(this, arguments);
      return this;
    };
  },
  setindex: function(Class, Native, orig){
    var types = {
      lexical: Native.ITLEXICAL,
      decimal: Native.ITDECIMAL,
      token:   Native.ITTOKEN,
      qgram:   Native.ITQGRAM,
      
      opt:     Native.ITOPT,
      'void':    Native.ITVOID,
      
      /* short */
      lex:     Native.ITLEXICAL,
      dec:     Native.ITDECIMAL,
      tok:     Native.ITTOKEN,
      qgr:     Native.ITQGRAM,
      
      /* flags */
      keep:    Native.ITKEEP,
      
      /* too short */
      l:       Native.ITLEXICAL,
      d:       Native.ITDECIMAL,
      t:       Native.ITTOKEN,
      q:       Native.ITQGRAM,
      
      o:       Native.ITOPT,
      v:       Native.ITVOID,
      
      k:       Native.ITKEEP,
      
      /* skip */
      ' ':       0,
      '/':       0,
      ',':       0
    };
    return function(){
      AStr2Bits(arguments, 1, types);
      orig.apply(this, arguments);
      return this;
    };
  },
  cur: {
    put: function(Class, Native, orig){
      var modes = {
        current: Native.CPCURRENT,
        before:  Native.CPBEFORE,
        after:   Native.CPAFTER,
        
        /* short */
        '@':     Native.CPCURRENT,
        '~':     Native.CPCURRENT,
        '^':     Native.CPBEFORE,
        '<':     Native.CPBEFORE,
        'v':     Native.CPAFTER,
        '>':     Native.CPAFTER,
        
        /* skip */
        ' ':     0,
        '/':     0,
        ',':     0
      };
      return function(){
        AStr2Bits(arguments, 1, modes);
        orig.apply(this, arguments);
        return this;
      };
    }
  },
  qry: {
    addcond: function(Class, Native, orig){
      var conds = {
        /* string */
        '~=': Native.QCSTREQ,   /* equal */
        '~@': Native.QCSTRINC,  /* include */
        '~^': Native.QCSTRBW,   /* begin */
        '~$': Native.QCSTREW,   /* end */
        '~&': Native.QCSTRAND,  /* all */
        '~|': Native.QCSTROR,   /* one */
        '~*': Native.QCSTROREQ, /* in (tokens) */
        '~?': Native.QCSTRRX,   /* regexp */

        /* number */
        '==': Native.QCNUMEQ,
        '>':  Native.QCNUMGT,
        '>=': Native.QCNUMGE,
        '<':  Native.QCNUMLT,
        '<=': Native.QCNUMLE,
        '<>': Native.QCNUMBT,   /* between */
        '><': Native.QCNUMBT,
        '=*': Native.QCNUMOREQ, /* in (tokens) */

        /* fulltext */
        '?~': Native.QCFTSPH,   /* phrase */
        '?&': Native.QCFTSAND,  /* all */
        '?|': Native.QCFTSOR,   /* one */
        '?*': Native.QCFTSEX,   /* compound */

        /* flags */
        '!': Native.QCNEGATE,   /* negate */
        '%': Native.QCNOIDX,    /* no index */

        /* short */
        '=': function(){ return isStr(arguments[2]) ? '~=' : '=='; },
        '*': function(){ return isStr(arguments[2]) ? '~*' : '=*'; },

        /* string */
        '@':  Native.QCSTRINC,  /* include */
        '^':  Native.QCSTRBW,   /* begin */
        '$':  Native.QCSTREW,   /* end */
        '&':  Native.QCSTRAND,  /* all */
        '|':  Native.QCSTROR,   /* one */
        '~':  Native.QCSTRRX,   /* regexp */

        /* skip */
        ' ':      0,
        '/':      0,
        ',':      0
      };
      return function(){
        AStr2Bits(arguments, 1, conds);
        orig.apply(this, arguments);
        return this;
      };
    },
    setorder: function(Class, Native, orig){
      var ords = {
        '~>': Native.QOSTRASC,
        '~<': Native.QOSTRDESC,
        '>':  Native.QONUMASC,
        '<':  Native.QONUMDESC,

        '~v':  Native.QOSTRASC,
        '~^':  Native.QOSTRDESC,
        'v':  Native.QONUMASC,
        '^':  Native.QONUMDESC
      };
      return function(){
        AStr2Bits(arguments, 1, ords);
        orig.apply(this, arguments);
        return this;
      };
    },
    setlimit: function(Class, Native, orig){
      return function(){
        orig.apply(this, arguments);
        return this;
      };
    }
  }
};

var sync = {
  setmutex: 0,
  tune: 0,
  setcache: 0,
  setxmsiz: 0,
  setdfunit: 0
};

var OK = {};
OK[TC.ESUCCESS] = 0;
OK[TC.ENOREC] = 'norec';
OK[TC.EKEEP] = 'keep';

var fixes = {
  hooksTable: hooks,
  hooks: function(Class, Native, Proto, Table){
    forFunc(Proto, function(name, orig, pure){
      if(isFunc(Table[pure])){
        Proto[name] = Table[pure](Class, Native, orig, isAsync(name)) || orig;
      }
    });
  },
  syncTable: sync,
  sync: function(Class, Native, Proto, Table){
    forFunc(Proto, function(name, orig, pure){
      if(isSync(name) || name in Table){
        Proto[name] = function(){
          var ret = orig.apply(this, arguments),
          code = isFunc(this.ecode) ? this.ecode() : TC.ESUCCESS;
          if(!(code in OK)){
            throw new TCError(this, name, arguments, code);
          }
          return ret;
        };
      }
    });
  },
  async: function(Class, Native, Proto){
    forFunc(Proto, function(name, orig, pure){
      if(isAsync(name)){
        delete Proto[name];
        Proto[pure] = function(){
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
          return orig.apply(this, arguments);
        };
      }
    });
  },
  aliasTable: alias,
  alias: function(Class, Native, Proto, Table){
    var s, d;
    for(d in Table){
      s = Table[d];
      if(isStr(s)){
        if(s in Proto){
          Proto[d] = Proto[s];
        }/*
        if(s in Native){
          Native[d] = Native[s];
        }*/
      }
    }
  }
};

function applyFixes(Class, Native, Table, Ident){
  var n, Proto = Native.prototype;
  if(!isObj(Table)){
    return Native;
  }
  forFunc(Table, function(n, f){
    var SubTable = Table[n + 'Table'] || Table;
    f(Class, Native, Proto, SubTable);
    if(isObj(SubTable[Class])){
      f(Class, Native, Proto, SubTable[Class]);
    }
  });
  return Ident ? Native : arguments.callee(Class, Native, Table[Class], true);
}

'H,B,F,T,A'.split(',').forEach(function(Pref){
  var Name = Pref + 'DB',
  name = Name.toLowerCase();
  
  exports[name] = applyFixes(name, TC[Name], fixes);
});

'B:CUR,T:QRY'.split(',').forEach(function(Pref){
  Pref = Pref.split(':');
  var Name = Pref[0] + 'DB',
  name = Pref[1].toLowerCase(),
  Cons = TC[Name][name] = applyFixes(name, TC[Name + Pref[1]], fixes);
  
  TC[Name].prototype[name] = function(){
    return new Cons(this);
  };
});

exports.ver = TC.VERSION;
