#pragma once

#include "api/peer_connection_interface.h"

class WebRTCStreamer;

class PCObserver : public webrtc::PeerConnectionObserver {
public:
    PCObserver(WebRTCStreamer* streamer);

    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
    void OnAddStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
    void OnRemoveStream(webrtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
    void OnDataChannel(webrtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
    void OnRenegotiationNeeded() override;
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
    void OnIceCandidate(const webrtc::IceCandidate* candidate) override;
    void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
private:
    WebRTCStreamer* streamer_;
};