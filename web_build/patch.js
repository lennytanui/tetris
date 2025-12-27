Module['locateFile'] = function(path, prefix) {
  if (path.endsWith('.wasm')) {
    return '../wasm/' + path;
  }
  return prefix + path;
};