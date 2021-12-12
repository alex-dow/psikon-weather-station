const HtmlWebpackPlugin = require('html-webpack-plugin');
const path = require('path');
const mode = process.env.NODE_ENV || 'development';
const prod = mode === 'production';
const rootPath = __dirname;
const distPath = path.join(__dirname, 'dist');

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
    new HtmlWebpackPlugin()
  ],
  devtool: prod ? false : 'source-map',
  devServer: {
    hot: true
  }
}