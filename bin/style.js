"use strict";

module.exports = {
  "version": 4,
  "sprite": "https://www.mapbox.com/mapbox-gl-styles/sprites/sprite",
  "glyphs": "http://mapbox.s3.amazonaws.com/gl-glyphs-256/{fontstack}/{range}.pbf",
  "constants": {
    "@red": "#f00",
    "@land": "#eee",
    "@water": "#999",
    "@park": "#bda",
    "@road": "#fefefe",
    "@border": "#6d90ab",
    "@wood": "#33AA66",
    "@building": "#ddd",
    "@building_outline": "#ccc",
    "@text": "#000000",
    "@satellite_brightness_low": 0,
    "@satellite_brightness_high": 1,
    "@satellite_saturation": 0,
    "@satellite_spin": 0,
    "@satellite_contrast": 0,
    "@road_blur": 1,
    "@stroke_width": 0.25
  },
  "sources": {
    "mapbox": {
      "type": "vector",
      "url": "mapbox://mapbox.mapbox-streets-v5",
      "maxZoom": 14
    }
  },
  "layers": [{
    "id": "bg",
    "type": "background",
    "style": {
      "background-color": "@land",
      "transition-background-color": {
        "duration": 500
      }
    },
    "style.satellite": {
      "transition-fill-color": {
        "duration": 500,
        "delay": 500
      },
      "fill-opacity": 0,
      "fill-color": "rgba(255,0,0,0)"
    }
  }, {
    "id": "park",
    "source": "mapbox",
    "source-layer": "landuse",
    "filter": { "class": "park" },
    "type": "fill",
    "style": {
      "fill-color": "@park"
    },
    "style.satellite": {
      "transition-fill-color": {
        "duration": 500
      },
      "fill-color": "rgba(0,0,0,0)"
    }
  }, {
    "id": "wood",
    "source": "mapbox",
    "source-layer": "landuse",
    "filter": { "class": "wood" },
    "type": "fill",
    "style": {
      "fill-color": "@wood",
      "fill-opacity": 0.08
    }
  }, {
    "id": "water",
    "source": "mapbox",
    "source-layer": "water",
    "type": "fill",
    "style": {
      "fill-color": "@water"
    },
    "style.satellite": {
      "fill-opacity": 0
    }
  }, {
    "id": "waterway",
    "source": "mapbox",
    "source-layer": "waterway",
    "type": "line",
    "style": {
      "line-color": "@water",
      "line-width": {
        "base": 1.01,
        "stops": [[7, 0.5], [19, 6.5]]
      }
    }
  }, {
  // "id": "roads",
  //   "layers": [{
      "id": "tunnel_large_casing",
      "source": "mapbox",
      "source-layer": "tunnel",
      "filter": { "class": ["motorway", "main"] },
      "type": "line",
      "style": {
        "line-color": "rgba(0,0,0,0.5)",
        "line-width": 1,
        "line-offset": {
          "stops": [[12.11458208048225, 1], [19, 93.2862864971161]]
        }
      }
    }, {
      "id": "tunnel_regular_casing",
      "source": "mapbox",
      "source-layer": "tunnel",
      "filter": { "class": ["street", "street_limited"] },
      "type": "line",
      "style": {
        "line-color": "rgba(0,0,0,0.5)",
        "line-width": 1,
        "line-offset": {
          "stops": [[11.637356828788917, 1], [19, 31.287358856201173]]
        }
      }
    }, {
      "id": "tunnel_large",
      "ref": "tunnel_large_casing",
      "style": {
        "line-color": "rgba(255,255,255,0.5)",
        "line-width": {
          "stops": [[12.11458208048225, 1], [19, 93.2862864971161]]
        }
      }
    }, {
      "id": "tunnel_regular",
      "ref": "tunnel_regular_casing",
      "style": {
        "line-color": "rgba(255,255,255,0.5)",
        "line-width": {
          "stops": [[14.11458208048225, 1], [19, 29.787358856201173]]
        }
      }
    }, {
      "id": "road_large_casing",
      "source": "mapbox",
      "source-layer": "road",
      "filter": { "class": ["motorway", "main"] },
      "type": "line",
      "render": {
        "line-cap": "round",
        "line-join": "bevel"
      },
      "style": {
        "line-color": "rgba(154,154,154,0.5)",
        "line-width": {
          "stops": [[12.751938909271168, 4], [19, 100.00060082197189]]
        },
        "line-opacity": {
          "base": 1.01,
          "stops": [[13, 0], [14, 1]]
        },
        "transition-line-width": {
          "duration": 500
        },
        "line-blur": "@road_blur"
      },
      "style.satellite": {
        "line-width": {
          "stops": [[13.751938909271168, 4], [19, 57.57177189826965]]
        },
        "transition-line-width": {
          "duration": 500,
          "delay": 1000
        }
      },
      "style.test": {
        "line-width": {
          "stops": [[11.751938909271168, 4], [19, 174.25105143845082]]
        },
        "line-color": "rgba(255,0,0,1)",
        "transition-line-width": {
          "duration": 500
        },
        "transition-line-color": {
          "duration": 2000,
          "delay": 500
        }
      }
    }, {
      "id": "road_regular_casing",
      "source": "mapbox",
      "source-layer": "road",
      "filter": { "class": "street" },
      "type": "line",
      "render": {
        "line-cap": "round",
        "line-join": "bevel"
      },
      "style": {
        "line-color": "rgba(154,154,154,0.5)",
        "line-width": {
          "stops": [[10.637356828788917, 1], [19, 54.377877998352055]]
        },
        "line-opacity": {
          "base": 1.01,
          "stops": [[14.5, 0], [15.5, 1]]
        },
        "line-blur": "@road_blur"
      },
      "style.satellite": {
        "transition-line-width": {
          "duration": 500,
          "delay": 1000
        },
        "line-width": {
          "stops": [[11.637356828788917, 1], [19, 31.287358856201173]]
        }
      }
    }, {
      "id": "road_limited",
      "source": "mapbox",
      "source-layer": "road",
      "filter": { "class": "street_limited" },
      "type": "line",
      "render": {
        "line-cap": "round",
        "line-join": "bevel",
        "line-round-limit": 0.7
      },
      "style": {
        "line-dasharray": [10, 2],
        "line-color": "@road",
        "line-blur": "@road_blur",
        "line-width": {
          "stops": [[13.11458208048225, 1], [19, 52.877877998352055]]
        }
      }
    }, {
      "id": "road_large",
      "ref": "road_large_casing",
      "style": {
        "line-color": "@road",
        "line-blur": "@road_blur",
        "line-width": {
          "stops": [[12.11458208048225, 1], [19, 93.2862864971161]]
        }
      },
      "style.satellite": {
        "transition-line-width": {
          "duration": 500,
          "delay": 1000
        },
        "line-width": {
          "stops": [[13.11458208048225, 1], [19, 52.877877998352055]]
        }
      }
    }, {
      "id": "road_regular",
      "ref": "road_regular_casing",
      "style": {
        "line-color": "@road",
        "line-blur": "@road_blur",
        "line-width": {
          "stops": [[13.11458208048225, 1], [19, 52.877877998352055]]
        }
      },
      "style.satellite": {
        "transition-line-width": {
          "duration": 500,
          "delay": 1000
        },
        "line-width": {
          "stops": [[14.11458208048225, 1], [19, 29.787358856201173]]
        }
      }
    }, {
      "id": "path",
      "source": "mapbox",
      "source-layer": "road",
      "filter": { "class": "path" },
      "type": "line",
      "render": {
        "line-cap": "round",
        "line-join": "bevel"
      },
      "style": {
        "line-color": "rgba(255,255,255,1)",
        "line-dasharray": [2, 2],
        "line-width": 2
      }
    }, {
      "id": "rail",
      "source": "mapbox",
      "source-layer": "road",
      "filter": { "class": "major_rail" },
      "type": "line",
      "render": {
        "line-cap": "round",
        "line-join": "bevel"
      },
      "style": {
        "line-color": "rgba(76,76,76,0.8)",
        "line-dasharray": [2, 1],
        "line-width": 3
      }
    }, {
      "id": "tunnel_rail",
      "source": "mapbox",
      "source-layer": "tunnel",
      "filter": { "class": ["minor_rail", "major_rail"] },
      "type": "line",
      "style": {
        "line-color": "rgba(76,76,76,0.3)",
        "line-dasharray": [2, 1],
        "line-width": 3
      }
    // }],
    // "type": "composite",
    // "style": {
    //   "composite-opacity": 1,
    //   "transition-composite-opacity": {
    //     "duration": 500
    //   }
    // },
    // "style.satellite": {
    //   "transition-composite-opacity": {
    //     "duration": 500,
    //     "delay": 500
    //   },
    //   "composite-opacity": 0.5
    // }
  }, {
    "id": "building",
    "source": "mapbox",
    "source-layer": "building",
    "type": "fill",
    "style": {
      "fill-color": "@building",
      "transition-fill-opacity": {
        "duration": 500,
        "delay": 500
      },
      "fill-opacity": {
        "base": 1.01,
        "stops": [[13, 0], [14, 1]]
      },
      "fill-outline-color": "@building_outline"
    },
    "style.satellite": {
      "fill-opacity": 0,
      "transition-fill-opacity": {
        "duration": 500
      }
    }
  }, {
    "id": "borders",
    "source": "mapbox",
    "source-layer": "admin",
    "type": "line",
    "style": {
      "line-color": "rgba(0,0,0,0.3)",
      "line-width": 1
    }
  }, {
    "id": "bridge_large_casing",
    "source": "mapbox",
    "source-layer": "bridge",
    "filter": { "class": ["motorway", "main"] },
    "type": "line",
    "style": {
      "line-color": "rgba(0,0,0,0.4)",
      "line-width": {
        "stops": [[2.6468052936710817, 1.51], [19, 95.7862864971161]]
      }
    }
  }, {
    "id": "bridge_large",
    "ref": "bridge_large_casing",
    "style": {
      "line-color": "@road",
      "line-width": {
        "stops": [[12.11458208048225, 1], [19, 93.2862864971161]]
      }
    }
  }, {
    "id": "park_poi",
    "source": "mapbox",
    "source-layer": "poi_label",
    "filter": { "maki": "park" },
    "type": "symbol",
    "render": {
      "icon-image": "dot.sdf",
      "icon-allow-overlap": true,
      "icon-ignore-placement": true
    },
    "style": {
      "icon-color": "#8f8",
      "icon-halo-color": "#393",
      "icon-halo-width": 0,
      "icon-halo-blur": 5,
      "icon-opacity": 0,
      "icon-halo-opacity": 0.9,
      "icon-translate": [2, -1],
      "icon-size": 15
    }
  }, {
    "id": "country_label",
    "source": "mapbox",
    "source-layer": "country_label",
    "filter": { "$type": "Point" },
    "type": "symbol",
    "render": {
      "text-field": "{name}",
      "text-font": "Open Sans Regular, Arial Unicode MS Regular",
      "text-max-size": 16,
      "text-path": "horizontal",
      "text-padding": 10
    },
    "style": {
      "text-halo-color": "rgba(255,255,255,0.7)",
      "text-halo-width": "@stroke_width",
      "text-color": "@text"
    }
  }, {
    "id": "place_label",
    "source": "mapbox",
    "source-layer": "place_label",
    "filter": { "$type": "Point" },
    "type": "symbol",
    "render": {
      "text-field": "{name}",
      "text-font": "Open Sans Semibold, Arial Unicode MS Regular",
      "text-max-size": 18,
      "text-path": "horizontal",
      "text-max-width": 2
    },
    "style": {
      "text-halo-color": "rgba(255,255,255,0.7)",
      "text-halo-width": "@stroke_width",
      "text-color": "@text"
    }
  }, {
    "id": "road_label",
    "source": "mapbox",
    "source-layer": "road_label",
    "filter": { "$type": "LineString" },
    "type": "symbol",
    "min-zoom": 14,
    "render": {
      "symbol-placement": "line",
      "text-field": "{name}",
      "text-font": "Open Sans Regular, Arial Unicode MS Regular",
      "text-max-size": 12,
      "text-path": "curve",
      "text-min-distance": 250,
      "text-max-angle": 1.04
    },
    "style": {
      "text-color": "@text",
      "text-halo-color": "rgba(255,255,255,0.7)",
      "text-halo-width": "@stroke_width",
      "text-size": {
        "stops": [[4.770835839035499, 8.01], [15.477225251693334, 12]]
      }
    }
  }, {
    "id": "poi",
    "source": "mapbox",
    "source-layer": "poi_label",
    "type": "symbol",
    "min-zoom": 15,
    "render": {
      "icon-size": 12,
      "icon-image": "{maki}-12",
      "text-font": "Open Sans Regular, Arial Unicode MS Regular",
      "text-max-size": 10,
      "text-offset": [0, 0.8],
      "text-path": "horizontal",
      "text-max-width": 7,
      "text-optional": true,
      "text-vertical-align": "top",
      "text-field": "{name}"
    },
    "style": {
      "text-size": 10,
      "text-color": "#333",
      "text-halo-color": "#eee",
      "icon-rotate-anchor": "viewport"
    }
  }, {
    "id": "road_markers",
    "source": "mapbox",
    "source-layer": "road",
    "filter": { "oneway": 1, "$type": "LineString" },
    "type": "symbol",
    "min-zoom": 16,
    "render": {
      "symbol-placement": "line",
      "symbol-min-distance": 150,
      "icon-image": "triangle-stroked-12",
      "icon-rotate": 90
    }
  }
  ]
};
