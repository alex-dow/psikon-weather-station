const HtmlWebpackPlugin = require('html-webpack-plugin');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');
const path = require('path');
const mode = process.env.NODE_ENV || 'development';
const prod = mode === 'production';

module.exports = {
  entry: 'src/main.js',
  resolve: {
    extensions: ['.mjs','.js','.svelte'],
    mainFields: ['svelte','browser','module','main']
  },
  output: {
    path: path.join(__dirname, '/dist'),
    filename: '[name].[ext]',
    chunkFilename: '[name].[id].[ext]'
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
    new HtmlWebpackPlugin(),
    new MiniCssExtractPlugin({
      filename: '[name].css'
    })
  ],
  devtool: prod ? false : 'source-map',
  devServer: {
    hot: true
  }
}