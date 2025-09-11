#pragma once

#include "pc/video_track_source.h"
#include "modules/video_capture/video_capture.h"

#include "absl/types/optional.h"

#include <iostream>

class CameraVideoTrackSource : public webrtc::VideoTrackSource {
public:
    static webrtc::scoped_refptr<CameraVideoTrackSource> Create(int camera_index);

    explicit CameraVideoTrackSource(int camera_index);

    bool is_screencast() const override { return false; }
    absl::optional<bool> needs_denoising() const override { return false; }

    webrtc::MediaSourceInterface::SourceState state() const override {
        return webrtc::MediaSourceInterface::kLive;
    }

    bool remote() const override { return false; }

    void StartCapture();
    void StopCapture();
    void OnIncomingCaptureFrame(const int32_t id, const webrtc::VideoFrame& videoFrame) override;
    void OnCaptureDelayed(const int32_t id, const int32_t delay) override;

private:
    int camera_index;
    webrtc::scoped_refptr<webrtc::VideoCaptureModule> video_capture_module_;
};