import React, { useState , useEffect, useCallback } from "react";

import AudioPlayer from "../components/AudioPlayer";

import ModuleJS from "../../core/core.js";
import ModuleWASM from "../../core/core.wasm";
import ModuleDATA from "../../core/core.data";

import Slider from "../components/Slider";
import SliderArray from "../components/SliderArray";

import "../styles/features/App.css";

/*----------------------------------------------------------------------------*/

export default function App(): React.ReactNode {

    const [core, set_core] = useState<any>(null);
    const [playhead_position, set_playhead_position]    = useState<number>(0);
    const [input_gain, set_input_gain]                  = useState<number>(1);
    const [output_gain, set_output_gain]                = useState<number>(1);
    
    /*------------------------------------------------------------------------*/
    
    useEffect(() => {
        if (core !== null) { return; }
        ModuleJS({locateFile: (path: string) => {
            if (path.endsWith(".wasm")) { return ModuleWASM; }
            else if (path.endsWith(".data")) { return ModuleDATA; }
            return path;
        }}).then(set_core);
    }, []);
    
    /*------------------------------------------------------------------------*/
    
    useEffect(() => {
        if (core === null) { return; }
        core._api_initialize();
        core._api_set_playhead_position_change_callback(core.addFunction(set_playhead_position, "vf"));
        return () => { core._api_terminate(); }
    }, [core]);
    
    /*------------------------------------------------------------------------*/
    
    const on_file_change = async (event: React.ChangeEvent<HTMLInputElement, HTMLInputElement>) => {
        if (!event.target.files) { return; }
        const file = event.target.files[0];

        const audio_context = new AudioContext();
        const array_buffer  = await file.arrayBuffer();
        const audio_buffer  = await audio_context.decodeAudioData(array_buffer);

        const l_ptr = core._api_allocate_buffer(audio_buffer.length * 2);
        const r_ptr = core._api_allocate_buffer(audio_buffer.length * 2);

        /**
         * Addresses in HEAPF32 are already multiplied by the float size,
         * so to align everything the address needs to be shifted 2 bits
         * to the right:
         * - HEAPF32 address 0 => real address 0
         * - HEAPF32 address 1 => read address 4
         * - HEAPF32 address 2 => read address 8
         * ...
         */
        core.HEAPF32.set(audio_buffer.getChannelData(0), l_ptr >> 2);
        core.HEAPF32.set(audio_buffer.getChannelData(1), r_ptr >> 2);

        core._api_play(l_ptr, r_ptr, audio_buffer.length);

        audio_context.close();
    }

    /*------------------------------------------------------------------------*/
    
    return (
        <div className="--fullscreen --background app-container">
            <canvas className="gl_canvas" id="gl_canvas"/>
            <div className="--flex-vertical">
                <div className="--flex-horizontal">
                    <div className="--border --round-corners app-audio-controls app-input">
                        <Slider
                            min_value={-100}
                            max_value={100}
                            default_value={0}
                            vertical={true}
                            on_input={(value: number) => {
                                core._api_set_input_gain(value + 1.0);  
                            }}
                        />
                    </div>
                    <div className="--border --round-corners app-audio-controls app-equalizer">
                        <SliderArray
                            label="EQUALIZER"
                            num_bands={10}
                            on_band_change={(band_index: number, band_value: number) => {
                                core._api_set_eq_band_gain((band_value + 1.0), band_index);
                            }
                            }
                        />
                    </div>
                    <div className="--border --round-corners app-audio-controls app-input">
                        <Slider
                            min_value={-100}
                            max_value={100}
                            default_value={0}
                            vertical={true}
                            on_input={(value: number) => core._api_set_output_gain(value + 1.0) }
                        />
                    </div>
                </div>
                <AudioPlayer
                    playhead_position={playhead_position}
                    on_play={() => {}}
                    on_pause={() => {}}
                    on_set_playhead_position={(position: number) => {
                        core._api_set_playhead_position(position);
                    }}
                />
                <div className="upload">
                    <input type="file" onChange={on_file_change}/>
                </div>
            </div>
        </div>
    );
}

/*----------------------------------------------------------------------------*/