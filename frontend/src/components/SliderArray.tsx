import React from "react";

import Slider from "./Slider";

import "../styles/components/SliderArray.css";

export default function SliderArray({
    label,
    num_bands,
    on_band_change
}: {
    label: string,
    num_bands: number,
    on_band_change: (band_id: number, band_value: number) => void
}): React.ReactNode {
    return (
        <div className="--background --flex-vertical">
            <div className="--background --flex-horizontal">
                {
                    Array.from(Array(num_bands).keys()).map((band_index) =>
                        <Slider
                            key={band_index}
                            min_value={-100}
                            max_value={100}
                            default_value={0}
                            vertical={true}
                            on_input={(value: number) => {
                                on_band_change(band_index, value);
                            }}
                        />
                    )
                }
            </div>
            <div className="--flex-vertical slider-array-label-container">
                <h1 className="slider-array-label">
                    {label}
                </h1>
            </div>
        </div>
    )
}