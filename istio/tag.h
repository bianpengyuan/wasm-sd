#ifndef ISTIO_TAG_H_
#define ISTIO_TAG_H_

#include "opencensus/stats/tag_key.h"

namespace istio {
namespace tag {

#define REGISTER_TAG(_t, _f) \
  opencensus::stats::TagKey _f##Key() {  \
    static const auto _t##_key =  \
        opencensus::stats::TagKey::Register(#_t);  \
    return _t##_key;  \
  }

REGISTER_TAG(request_latency, RequestLatency)
REGISTER_TAG(request_protocol, RequestProtocol)
REGISTER_TAG(mesh_uid, MeshUID)
REGISTER_TAG(destination_service_name, DestinationServiceName)
REGISTER_TAG(destination_service_namespace, DestinationServiceNamespace)
REGISTER_TAG(destination_port, DestinationPort)
REGISTER_TAG(source_principal, SourcePrincipal)
REGISTER_TAG(source_workload_name, SourceWorkloadName)
REGISTER_TAG(source_workload_namespace, SourceWorkloadNamespace)
REGISTER_TAG(source_owner, SourceOwner)
REGISTER_TAG(destination_workload_name, DestinationWorkloadName)
REGISTER_TAG(destination_workload_namespace, DestinationWorkloadNamespace)
REGISTER_TAG(destination_owner, DestinationOwner)

}  // namespace tag
}  // measure istio


#endif // ISTIO_TAG_H_