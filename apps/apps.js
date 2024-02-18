'use strict';

angular.module('nwas').service('apps', function() {
  return [
    { name: "Nmg2", description: {en: "Numengine2D", fr: "Numengine2D"} },
    { name: "Nmg3", description: {en: "Numengine3D", fr: "Numengine3D"} },
  ];
});
