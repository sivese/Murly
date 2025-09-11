#include <iostream>

#include "streaming/streamer.hh"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"

WebRTCStreamer::WebRTCStreamer() = default;

WebRTCStreamer::~WebRTCStreamer() {
    stopStreaming();
}

bool WebRTCStreamer::initialize() {
    signaling_thread_   = webrtc::Thread::Create();
    worker_thread_      = webrtc::Thread::Create();
    network_thread_     = webrtc::Thread::Create();

    signaling_thread_->SetName("signaling", nullptr);
    worker_thread_->SetName("worker", nullptr);
    network_thread_->SetName("network", nullptr);

    if(!signaling_thread_->Start() || !worker_thread_->Start() || !network_thread_->Start()) {
        std::cerr<<"Failed to start threads"<<std::endl;
        return false;
    }

    peer_connection_factory_ = webrtc::CreatePeerConnectionFactory(
        network_thread_.get(), worker_thread_.get(), signaling_thread_.get(),
        nullptr /* default adm */, 
        webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory(),
        webrtc::CreateBuiltinVideoEncoderFactory(),
        webrtc::CreateBuiltinVideoDecoderFactory(), 
        nullptr /* audio_mixer */,
        nullptr /* audio_processing */
    );

    if(!peer_connection_factory_) {
        std::cerr<<"Failed to create PeerConnectionFactory"<<std::endl;
        return false;
    }

    std::cout<<"WebRTC Streamer initialized successfully"<<std::endl;

    return true;
}

bool WebRTCStreamer::createPeerConnection() {
    webrtc::PeerConnectionInterface::RTCConfiguration config;
    webrtc::PeerConnectionInterface::IceServer ice_server;

    ice_server.uri = "stun:stun.l.google.com:19302";
    config.servers.push_back(ice_server);

    pc_observer_ = std::make_unique<PCObserver>(this);

    return true;
}