const Path					= require('path')
const Binary				= require('@mapbox/node-pre-gyp')
const binding_path			= Binary.find(Path.resolve(Path.join(__dirname,'../package.json')))
const Binding				= require(binding_path)

module.exports = Binding