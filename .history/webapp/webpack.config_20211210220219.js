const HtmlWebpackPlugin = require('html-webpack-plugin');
const HtmlWebpackInlineSourcePlugin = require('html-webpack-inline-source-plugin');
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
          emitCss: prod,
          hotReload: !prod
        }
      }
    },{
      test: /\.css$/,
      use: [
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
    new webpack.DefinePlugin({
      isProduction: prod
    }),
    new HtmlWebpackPlugin({
      title: 'Psikon Weather Station',
      inlineSource: '.(js|css)$'
    }),
    new HtmlWebpackInlineSourcePlugin()
  ],
  devtool: 'source-map',
  devServer: {
    hot: true
  }
}