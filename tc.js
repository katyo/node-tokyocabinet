var TC = require('./tc_all'),
UT = require('util');

var reSync = /Sync$/,
isSync = function(name){
  return reSync.test(name);
},
deSync = function(name){
  return name.replace(reSync, '');
},
reAsync = /Async$/,
isAsync = function(name){
  return reAsync.test(name);
},
deAsync = function(name){
  return name.replace(reAsync, '');
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
  if(typeof self.errmsg == 'function'
     && typeof self.ecode == 'function'){
    this.message = self.errmsg(self.ecode());
  }
  if(func){
    this.message += '\n   call: ' + func + '(' + FArgs(args) + ')';
  }
};
TCError.prototype.__proto__ = Error.prototype;

function Class(Native){
  var n,
  Proto = Native.prototype;

  for(n in Proto){
    (function(name, data, name_){
      if(isFunc(data)){
        if(isSync(name)){
          Proto[name] = function(){
            var ret = data.apply(this, arguments);
            if(typeof this.ecode == 'function' && this.ecode()){
              throw new TCError(this, name, arguments);
            }
            return ret;
          };
        }
        if(isAsync(name)){
          delete Proto[name];
          name_ = deAsync(name);
          Proto[name_] = function(){
            if(arguments.length > 0 && isFunc(arguments[arguments.length - 1])){
              var callback = arguments[arguments.length - 1],
              args = arguments,
              self = this;
              arguments[arguments.length - 1] = function(){
                if(arguments[0]){
                  arguments[0] = new TCError(self, name_, args);
                }
                callback.apply(this, arguments);
              };
            }
            data.apply(this, arguments);
          };
        }
      }
    })(n, Proto[n]);
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
exports.tdb.qry = Class(TC.TDBQRY);
