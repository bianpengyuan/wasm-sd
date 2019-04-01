namespace wasmsd {
namespace stats {

void Record(std::initializer_list<Measurement> measurements,
            opencensus::tags::TagMap tags) {
    DeltaProducer::Get()->Record(measurements, std::move(tags));
}

}
}