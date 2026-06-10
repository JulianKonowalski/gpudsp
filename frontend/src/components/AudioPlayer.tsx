import React, { useState } from "react";

import Slider from "./Slider";

import "../styles/components/AudioPlayer.css";

export default function AudioPlayer({
    playhead_position,
    on_play,
    on_pause,
    on_set_playhead_position
}: {
    playhead_position: number,
    on_play: () => void,
    on_pause: () => void,
    on_set_playhead_position: (position: number) => void
}): React.ReactNode {
    return (
        <div className="audio-player-container">
            <Slider
                min_value={0}
                max_value={1000}
                default_value={0}
                vertical={false}
                on_input={on_set_playhead_position}
            />
        </div>
    );
}