const HtmlWebpackPlugin = require('html-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const path = require('path');
const mode = process.env.NODE_ENV || 'development';
const prod = mode === 'production';
const rootPath = __dirname;
const distPath = path.join(__dirname, 'dist');
const webpack = require('webpack');

module.exports = {
  entry: path.join(rootPath,'src','main.js'),
  resolve: {
    extensions: ['.mjs','.js','.svelte'],
    mainFields: ['svelte','browser','module','main']
  },
  output: {
    path: distPath
  },
  module: {
    rules: [{
      test: /\.svelte$/,
      use: {
        loader: 'svelte-loader',
        options: {
          compilerOptions: {
            dev: !prod
          },
          emitCss: true,
          hotReload: !prod
        }
      }
    },{
      test: /\.css$/,
      use: [
        MiniCssExtractPlugin.loader,
        'css-loader'
      ]
    },{
      test: /node_modules\/svelte\/.*\.mjs$/,
      resolve: {
        fullySpecified: false
      }
    }]
  },
  mode,
  plugins: [
    new MiniCssExtractPlugin(),
    new webpack.DefinePlugin({
      isProduction: prod
    }),
    new HtmlWebpackPlugin({
      title: 'Psikon Weather Station'
    }),
  ],
  devtool: 'source-map',
  devServer: {
    hot: true,
    client: {
      overlay: false
    }
  }
}