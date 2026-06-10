const HtmlWebpackPlugin = require("html-webpack-plugin");

module.exports = {
    context: __dirname,
    entry: "./src/index.tsx",
    mode: "development",
    devServer: {
        historyApiFallback: true,
        headers: {
            "Cross-Origin-Opener-Policy": "same-origin",
            "Cross-Origin-Embedder-Policy": "require-corp"
        }
    },
    resolve: {
        extensions: [".tsx", ".jsx", ".js", ".ts"]
    },
    module: {
        rules: [
            {
                test: /\.jsx$/,
                exclude: "/node_modules/",
                loader: "babel-loader",
                options: {
                    presets: ["@babel/preset-react"],
                }
            },
            {
                test: /\.tsx$/,
                exclude: "/node_modules/",
                use: "ts-loader"
            },
            {
                test: /\.js$/,
                type: "javascript/auto"
            },
            {
                test: /\.wasm$/,
                loader: "file-loader",
                type: "javascript/auto"
            },
            {
                test: /\.data$/,
                type: "asset/resource"
            },
            {
                test: /\.css$/i,
                use: ["style-loader", "css-loader"],
                type: "javascript/auto"
            }
        ],
    },
    plugins: [
        new HtmlWebpackPlugin({ template: "./src/index.html" }),
    ],
};