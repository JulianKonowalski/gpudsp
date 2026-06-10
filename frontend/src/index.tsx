import React from "react";
import { createRoot } from "react-dom/client";

import App from "./features/App";

import "./styles/index.css";

/*----------------------------------------------------------------------------*/

const root: HTMLElement | null = document.getElementById("root");
if (root) { createRoot(root).render(<App/>); }

/*----------------------------------------------------------------------------*/