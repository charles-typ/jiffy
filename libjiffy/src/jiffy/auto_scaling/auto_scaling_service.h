/**
 * Autogenerated by Thrift Compiler (0.12.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef auto_scaling_service_H
#define auto_scaling_service_H

#include <thrift/TDispatchProcessor.h>
#include <thrift/async/TConcurrentClientSyncInfo.h>
#include "auto_scaling_service_types.h"

namespace jiffy { namespace auto_scaling {

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning (disable : 4250 ) //inheriting methods via dominance 
#endif

class auto_scaling_serviceIf {
 public:
  virtual ~auto_scaling_serviceIf() {}
  virtual void auto_scaling(const std::vector<std::string> & current_replica_chain, const std::string& path, const std::map<std::string, std::string> & conf) = 0;
};

class auto_scaling_serviceIfFactory {
 public:
  typedef auto_scaling_serviceIf Handler;

  virtual ~auto_scaling_serviceIfFactory() {}

  virtual auto_scaling_serviceIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) = 0;
  virtual void releaseHandler(auto_scaling_serviceIf* /* handler */) = 0;
};

class auto_scaling_serviceIfSingletonFactory : virtual public auto_scaling_serviceIfFactory {
 public:
  auto_scaling_serviceIfSingletonFactory(const ::apache::thrift::stdcxx::shared_ptr<auto_scaling_serviceIf>& iface) : iface_(iface) {}
  virtual ~auto_scaling_serviceIfSingletonFactory() {}

  virtual auto_scaling_serviceIf* getHandler(const ::apache::thrift::TConnectionInfo&) {
    return iface_.get();
  }
  virtual void releaseHandler(auto_scaling_serviceIf* /* handler */) {}

 protected:
  ::apache::thrift::stdcxx::shared_ptr<auto_scaling_serviceIf> iface_;
};

class auto_scaling_serviceNull : virtual public auto_scaling_serviceIf {
 public:
  virtual ~auto_scaling_serviceNull() {}
  void auto_scaling(const std::vector<std::string> & /* current_replica_chain */, const std::string& /* path */, const std::map<std::string, std::string> & /* conf */) {
    return;
  }
};

typedef struct _auto_scaling_service_auto_scaling_args__isset {
  _auto_scaling_service_auto_scaling_args__isset() : current_replica_chain(false), path(false), conf(false) {}
  bool current_replica_chain :1;
  bool path :1;
  bool conf :1;
} _auto_scaling_service_auto_scaling_args__isset;

class auto_scaling_service_auto_scaling_args {
 public:

  auto_scaling_service_auto_scaling_args(const auto_scaling_service_auto_scaling_args&);
  auto_scaling_service_auto_scaling_args& operator=(const auto_scaling_service_auto_scaling_args&);
  auto_scaling_service_auto_scaling_args() : path() {
  }

  virtual ~auto_scaling_service_auto_scaling_args() throw();
  std::vector<std::string>  current_replica_chain;
  std::string path;
  std::map<std::string, std::string>  conf;

  _auto_scaling_service_auto_scaling_args__isset __isset;

  void __set_current_replica_chain(const std::vector<std::string> & val);

  void __set_path(const std::string& val);

  void __set_conf(const std::map<std::string, std::string> & val);

  bool operator == (const auto_scaling_service_auto_scaling_args & rhs) const
  {
    if (!(current_replica_chain == rhs.current_replica_chain))
      return false;
    if (!(path == rhs.path))
      return false;
    if (!(conf == rhs.conf))
      return false;
    return true;
  }
  bool operator != (const auto_scaling_service_auto_scaling_args &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const auto_scaling_service_auto_scaling_args & ) const;

  template <class Protocol_>
  uint32_t read(Protocol_* iprot);
  template <class Protocol_>
  uint32_t write(Protocol_* oprot) const;

};


class auto_scaling_service_auto_scaling_pargs {
 public:


  virtual ~auto_scaling_service_auto_scaling_pargs() throw();
  const std::vector<std::string> * current_replica_chain;
  const std::string* path;
  const std::map<std::string, std::string> * conf;

  template <class Protocol_>
  uint32_t write(Protocol_* oprot) const;

};

template <class Protocol_>
class auto_scaling_serviceClientT : virtual public auto_scaling_serviceIf {
 public:
  auto_scaling_serviceClientT(apache::thrift::stdcxx::shared_ptr< Protocol_> prot) {
    setProtocolT(prot);
  }
  auto_scaling_serviceClientT(apache::thrift::stdcxx::shared_ptr< Protocol_> iprot, apache::thrift::stdcxx::shared_ptr< Protocol_> oprot) {
    setProtocolT(iprot,oprot);
  }
 private:
  void setProtocolT(apache::thrift::stdcxx::shared_ptr< Protocol_> prot) {
  setProtocolT(prot,prot);
  }
  void setProtocolT(apache::thrift::stdcxx::shared_ptr< Protocol_> iprot, apache::thrift::stdcxx::shared_ptr< Protocol_> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return this->piprot_;
  }
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return this->poprot_;
  }
  void auto_scaling(const std::vector<std::string> & current_replica_chain, const std::string& path, const std::map<std::string, std::string> & conf);
  void send_auto_scaling(const std::vector<std::string> & current_replica_chain, const std::string& path, const std::map<std::string, std::string> & conf);
 protected:
  apache::thrift::stdcxx::shared_ptr< Protocol_> piprot_;
  apache::thrift::stdcxx::shared_ptr< Protocol_> poprot_;
  Protocol_* iprot_;
  Protocol_* oprot_;
};

typedef auto_scaling_serviceClientT< ::apache::thrift::protocol::TProtocol> auto_scaling_serviceClient;

template <class Protocol_>
class auto_scaling_serviceProcessorT : public ::apache::thrift::TDispatchProcessorT<Protocol_> {
 protected:
  ::apache::thrift::stdcxx::shared_ptr<auto_scaling_serviceIf> iface_;
  virtual bool dispatchCall(::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, const std::string& fname, int32_t seqid, void* callContext);
  virtual bool dispatchCallTemplated(Protocol_* iprot, Protocol_* oprot, const std::string& fname, int32_t seqid, void* callContext);
 private:
  typedef  void (auto_scaling_serviceProcessorT::*ProcessFunction)(int32_t, ::apache::thrift::protocol::TProtocol*, ::apache::thrift::protocol::TProtocol*, void*);
  typedef void (auto_scaling_serviceProcessorT::*SpecializedProcessFunction)(int32_t, Protocol_*, Protocol_*, void*);
  struct ProcessFunctions {
    ProcessFunction generic;
    SpecializedProcessFunction specialized;
    ProcessFunctions(ProcessFunction g, SpecializedProcessFunction s) :
      generic(g),
      specialized(s) {}
    ProcessFunctions() : generic(NULL), specialized(NULL) {}
  };
  typedef std::map<std::string, ProcessFunctions> ProcessMap;
  ProcessMap processMap_;
  void process_auto_scaling(int32_t seqid, ::apache::thrift::protocol::TProtocol* iprot, ::apache::thrift::protocol::TProtocol* oprot, void* callContext);
  void process_auto_scaling(int32_t seqid, Protocol_* iprot, Protocol_* oprot, void* callContext);
 public:
  auto_scaling_serviceProcessorT(::apache::thrift::stdcxx::shared_ptr<auto_scaling_serviceIf> iface) :
    iface_(iface) {
    processMap_["auto_scaling"] = ProcessFunctions(
      &auto_scaling_serviceProcessorT::process_auto_scaling,
      &auto_scaling_serviceProcessorT::process_auto_scaling);
  }

  virtual ~auto_scaling_serviceProcessorT() {}
};

typedef auto_scaling_serviceProcessorT< ::apache::thrift::protocol::TDummyProtocol > auto_scaling_serviceProcessor;

template <class Protocol_>
class auto_scaling_serviceProcessorFactoryT : public ::apache::thrift::TProcessorFactory {
 public:
  auto_scaling_serviceProcessorFactoryT(const ::apache::thrift::stdcxx::shared_ptr< auto_scaling_serviceIfFactory >& handlerFactory) :
      handlerFactory_(handlerFactory) {}

  ::apache::thrift::stdcxx::shared_ptr< ::apache::thrift::TProcessor > getProcessor(const ::apache::thrift::TConnectionInfo& connInfo);

 protected:
  ::apache::thrift::stdcxx::shared_ptr< auto_scaling_serviceIfFactory > handlerFactory_;
};

typedef auto_scaling_serviceProcessorFactoryT< ::apache::thrift::protocol::TDummyProtocol > auto_scaling_serviceProcessorFactory;

class auto_scaling_serviceMultiface : virtual public auto_scaling_serviceIf {
 public:
  auto_scaling_serviceMultiface(std::vector<apache::thrift::stdcxx::shared_ptr<auto_scaling_serviceIf> >& ifaces) : ifaces_(ifaces) {
  }
  virtual ~auto_scaling_serviceMultiface() {}
 protected:
  std::vector<apache::thrift::stdcxx::shared_ptr<auto_scaling_serviceIf> > ifaces_;
  auto_scaling_serviceMultiface() {}
  void add(::apache::thrift::stdcxx::shared_ptr<auto_scaling_serviceIf> iface) {
    ifaces_.push_back(iface);
  }
 public:
  void auto_scaling(const std::vector<std::string> & current_replica_chain, const std::string& path, const std::map<std::string, std::string> & conf) {
    size_t sz = ifaces_.size();
    size_t i = 0;
    for (; i < (sz - 1); ++i) {
      ifaces_[i]->auto_scaling(current_replica_chain, path, conf);
    }
    ifaces_[i]->auto_scaling(current_replica_chain, path, conf);
  }

};

// The 'concurrent' client is a thread safe client that correctly handles
// out of order responses.  It is slower than the regular client, so should
// only be used when you need to share a connection among multiple threads
template <class Protocol_>
class auto_scaling_serviceConcurrentClientT : virtual public auto_scaling_serviceIf {
 public:
  auto_scaling_serviceConcurrentClientT(apache::thrift::stdcxx::shared_ptr< Protocol_> prot) {
    setProtocolT(prot);
  }
  auto_scaling_serviceConcurrentClientT(apache::thrift::stdcxx::shared_ptr< Protocol_> iprot, apache::thrift::stdcxx::shared_ptr< Protocol_> oprot) {
    setProtocolT(iprot,oprot);
  }
 private:
  void setProtocolT(apache::thrift::stdcxx::shared_ptr< Protocol_> prot) {
  setProtocolT(prot,prot);
  }
  void setProtocolT(apache::thrift::stdcxx::shared_ptr< Protocol_> iprot, apache::thrift::stdcxx::shared_ptr< Protocol_> oprot) {
    piprot_=iprot;
    poprot_=oprot;
    iprot_ = iprot.get();
    oprot_ = oprot.get();
  }
 public:
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> getInputProtocol() {
    return this->piprot_;
  }
  apache::thrift::stdcxx::shared_ptr< ::apache::thrift::protocol::TProtocol> getOutputProtocol() {
    return this->poprot_;
  }
  void auto_scaling(const std::vector<std::string> & current_replica_chain, const std::string& path, const std::map<std::string, std::string> & conf);
  void send_auto_scaling(const std::vector<std::string> & current_replica_chain, const std::string& path, const std::map<std::string, std::string> & conf);
 protected:
  apache::thrift::stdcxx::shared_ptr< Protocol_> piprot_;
  apache::thrift::stdcxx::shared_ptr< Protocol_> poprot_;
  Protocol_* iprot_;
  Protocol_* oprot_;
  ::apache::thrift::async::TConcurrentClientSyncInfo sync_;
};

typedef auto_scaling_serviceConcurrentClientT< ::apache::thrift::protocol::TProtocol> auto_scaling_serviceConcurrentClient;

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

}} // namespace

#include "auto_scaling_service.tcc"
#include "auto_scaling_service_types.tcc"

#endif
