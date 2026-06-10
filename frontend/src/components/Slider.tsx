import React, { useState, useEffect, useCallback, useRef } from "react";

import "../styles/components/Slider.css";

/*----------------------------------------------------------------------------*/

export default function Slider({
    min_value,
    max_value,
    default_value,
    vertical,
    on_input,
}: {
    min_value: number,
    max_value: number,
    default_value: number,
    vertical: boolean,
    on_input: (value: number) => void
}): React.ReactNode {

    /*------------------------------------------------------------------------*/
    
    const input_ref = useRef<HTMLInputElement>(null);
    const [slider_value, set_slider_value] = useState<number>(default_value);

    /*------------------------------------------------------------------------*/

    const input_callback = useCallback(() => {
        const new_value = Number(input_ref.current!.value) / max_value;
        set_slider_value(new_value * max_value);
        on_input(new_value);
    }, [set_slider_value, on_input]);
        
    /*------------------------------------------------------------------------*/

    useEffect(() => {
        if (!input_ref.current) { return; }

        const double_click_callback = (e: MouseEvent) => {
            e.preventDefault();
            set_slider_value(default_value)
            on_input(default_value);
        };

        input_ref.current.addEventListener("dblclick", double_click_callback);
        return () => input_ref.current?.removeEventListener("dblclick", double_click_callback);
    }, [input_ref.current, set_slider_value]);

    /*------------------------------------------------------------------------*/

    return (
        <div className={`--flex-${vertical ? "vertical" : "horizontal"} slider-container`}>
            <input
                className={`slider ${vertical ? "vertical" : "horizontal"} --border --round-corners`}
                ref={input_ref}
                type="range"
                min={min_value}
                max={max_value}
                value={slider_value}
                onInput={input_callback}
            />
        </div>
    );

    /*------------------------------------------------------------------------*/
    
}

/*----------------------------------------------------------------------------*/