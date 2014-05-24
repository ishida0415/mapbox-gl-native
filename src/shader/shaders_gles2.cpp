// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
#include <llmr/platform/gl.hpp>
#ifdef GL_ES_VERSION_2_0
#include <llmr/shader/shaders.hpp>

using namespace llmr;

const shader_source llmr::shaders[SHADER_COUNT] = {
   {
       "precision highp float;\nattribute vec2 a_pos;\n\nuniform mat4 u_matrix;\nvarying vec2 v_pos;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n    v_pos = (gl_Position.xy + 1.0) / 2.0;\n}\n",
       "precision highp float;\nuniform sampler2D u_image;\nuniform float u_opacity;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    vec4 color = texture2D(u_image, v_pos);\n    gl_FragColor = color * u_opacity;\n}\n",
   },
   {
       "precision highp float;\nattribute vec2 a_pos;\n\nuniform mat4 u_matrix;\nuniform float u_size;\n\nvoid main(void) {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n    gl_PointSize = u_size;\n}\n",
       "precision highp float;\nuniform vec4 u_color;\nuniform float u_blur;\n\nvoid main() {\n\tfloat dist = length(gl_PointCoord - 0.5);\n\tfloat t = smoothstep(0.5, 0.5 - u_blur, dist);\n\n    gl_FragColor = u_color * t;\n}\n",
   },
   {
       "precision highp float;\nattribute vec2 a_pos;\nattribute vec2 a_tex;\n\nuniform mat4 u_matrix;\nuniform float u_size;\nuniform float u_ratio;\n\nvarying vec2 v_tex;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n    gl_PointSize = u_size;\n    v_tex = a_tex * u_ratio;\n}\n",
       "precision highp float;\nuniform sampler2D u_image;\nuniform vec2 u_dimension;\nuniform vec4 u_color;\nuniform float u_size;\n\nvarying vec2 v_tex;\n\nvoid main() {\n    vec2 pos = (v_tex + (gl_PointCoord - 0.5) * u_size) / u_dimension;\n    gl_FragColor = u_color * texture2D(u_image, pos);\n}\n",
   },
   {
       "precision highp float;\n// floor(127 / 2) == 63.0\n// the maximum allowed miter limit is 2.0 at the moment. the extrude normal is\n// stored in a byte (-128..127). we scale regular normals up to length 63, but\n// there are also \"special\" normals that have a bigger length (of up to 126 in\n// this case).\n// #define scale 63.0\n#define scale 0.015873016\n\nattribute vec2 a_pos;\nattribute vec2 a_extrude;\nattribute float a_linesofar;\n\n// posmatrix is for the vertex position, exmatrix is for rotating and projecting\n// the extrusion vector.\nuniform mat4 u_matrix;\nuniform mat4 u_exmatrix;\n\n// shared\nuniform float u_ratio;\nuniform vec2 u_linewidth;\nuniform vec4 u_color;\n\nvarying vec2 v_normal;\nvarying float v_linesofar;\n\nvoid main() {\n    // We store the texture normals in the most insignificant bit\n    // transform y so that 0 => -1 and 1 => 1\n    // In the texture normal, x is 0 if the normal points straight up/down and 1 if it's a round cap\n    // y is 1 if the normal points up, and -1 if it points down\n    vec2 normal = mod(a_pos, 2.0);\n    normal.y = sign(normal.y - 0.5);\n    v_normal = normal;\n\n    // Scale the extrusion vector down to a normal and then up by the line width\n    // of this vertex.\n    vec4 dist = vec4(u_linewidth.s * a_extrude * scale, 0.0, 0.0);\n\n    // Remove the texture normal bit of the position before scaling it with the\n    // model/view matrix. Add the extrusion vector *after* the model/view matrix\n    // because we're extruding the line in pixel space, regardless of the current\n    // tile's zoom level.\n    gl_Position = u_matrix * vec4(floor(a_pos * 0.5), 0.0, 1.0) + u_exmatrix * dist;\n    v_linesofar = a_linesofar * u_ratio;\n}",
       "precision highp float;\nuniform vec2 u_linewidth;\nuniform vec4 u_color;\n\nuniform vec2 u_dasharray;\n\nvarying vec2 v_normal;\nvarying float v_linesofar;\n\nvoid main() {\n    // Calculate the distance of the pixel from the line in pixels.\n    float dist = length(v_normal) * u_linewidth.s;\n\n    // Calculate the antialiasing fade factor. This is either when fading in\n    // the line in case of an offset line (v_linewidth.t) or when fading out\n    // (v_linewidth.s)\n    float alpha = clamp(min(dist - (u_linewidth.t - 1.0), u_linewidth.s - dist), 0.0, 1.0);\n\n    // Calculate the antialiasing fade factor based on distance to the dash.\n    // Only affects alpha when line is dashed\n    float pos = mod(v_linesofar, u_dasharray.x + u_dasharray.y);\n    alpha *= max(step(0.0, -u_dasharray.y), clamp(min(pos, u_dasharray.x - pos), 0.0, 1.0));\n\n    gl_FragColor = u_color * alpha;\n}",
   },
   {
       "precision highp float;\nattribute vec2 a_pos;\n\nuniform mat4 u_matrix;\nuniform vec2 u_world;\nuniform float u_size;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(floor(a_pos / 2.0), 0.0, 1.0);\n    v_pos = (gl_Position.xy + 1.0) * u_world;\n    gl_PointSize = u_size;\n}\n",
       "precision highp float;\nuniform vec4 u_color;\nuniform vec2 u_linewidth;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    float dist = length(v_pos - gl_FragCoord.xy);\n\n    // Calculate the antialiasing fade factor. This is either when fading in\n    // the line in case of an offset line (v_linewidth.t) or when fading out\n    // (v_linewidth.s)\n    float alpha = clamp(min(dist - (u_linewidth.t - 1.0), u_linewidth.s - dist), 0.0, 1.0);\n    gl_FragColor = u_color * alpha;\n}\n",
   },
   {
       "precision highp float;\nattribute vec2 a_pos;\nuniform mat4 u_matrix;\nuniform vec2 u_world;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n    v_pos = (gl_Position.xy + 1.0) / 2.0 * u_world;\n}\n",
       "precision highp float;\nuniform vec4 u_color;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    float dist = length(v_pos - gl_FragCoord.xy);\n    float alpha = smoothstep(1.0, 0.0, dist);\n    gl_FragColor = u_color * alpha;\n}\n",
   },
   {
       "precision highp float;\nuniform mat4 u_matrix;\n\nattribute vec2 a_pos;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    v_pos = a_pos;\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n}\n",
       "precision highp float;\nuniform vec4 u_color;\n\nuniform vec2 u_offset;\nuniform vec2 u_pattern_size;\nuniform vec2 u_pattern_tl;\nuniform vec2 u_pattern_br;\nuniform float u_mix;\n\n\nuniform sampler2D u_image;\n\nvarying vec2 v_pos;\n\nvoid main() {\n\n    vec2 imagecoord = mod((v_pos + u_offset) / u_pattern_size, 1.0);\n    vec2 pos = mix(u_pattern_tl, u_pattern_br, imagecoord);\n    vec4 color1 = texture2D(u_image, pos);\n\n    vec2 imagecoord2 = mod(imagecoord * 2.0, 1.0);\n    vec2 pos2 = mix(u_pattern_tl, u_pattern_br, imagecoord2);\n    vec4 color2 = texture2D(u_image, pos2);\n\n    vec4 color = mix(color1, color2, u_mix);\n    gl_FragColor = color + u_color * (1.0 - color.a);\n}\n",
   },
   {
       "precision highp float;\nattribute vec2 a_pos;\n\nuniform mat4 u_matrix;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n}\n",
       "precision highp float;\nuniform vec4 u_color;\n\nvoid main() {\n    gl_FragColor = u_color;\n}\n",
   },
   {
       "precision highp float;\nuniform mat4 u_matrix;\n\nattribute vec2 a_pos;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    gl_Position = u_matrix * vec4(a_pos, 0, 1);\n    v_pos = a_pos / 4096.0;\n}\n",
       "precision highp float;\nuniform sampler2D u_image;\nuniform float u_opacity;\n\nvarying vec2 v_pos;\n\nvoid main() {\n    gl_FragColor = texture2D(u_image, v_pos) * u_opacity;\n}\n",
   },
   {
       "precision highp float;\nattribute vec2 a_pos;\nattribute vec2 a_offset;\nattribute vec4 a_data1;\nattribute vec4 a_data2;\n\n\n// posmatrix is for the vertex position, exmatrix is for rotating and projecting\n// the extrusion vector.\nuniform mat4 u_matrix;\nuniform mat4 u_exmatrix;\nuniform float u_angle;\nuniform float u_zoom;\nuniform float u_flip;\nuniform float u_fadedist;\nuniform float u_minfadezoom;\nuniform float u_maxfadezoom;\nuniform float u_fadezoom;\n\nuniform vec2 u_texsize;\n\nvarying vec2 v_tex;\nvarying float v_alpha;\n\nvoid main() {\n    vec2 a_tex = a_data1.xy;\n    float a_labelminzoom = a_data1[2];\n    float a_angle = a_data1[3];\n    vec2 a_zoom = a_data2.st;\n    vec2 a_range = a_data2.pq;\n\n    float rev = 0.0;\n\n    // u_angle is angle of the map, -128..128 representing 0..2PI\n    // a_angle is angle of the label, 0..256 representing 0..2PI, where 0 is horizontal text\n    float rotated = mod(a_angle + u_angle, 256.0);\n    // if the label rotates with the map, and if the rotated label is upside down, hide it\n    if (u_flip > 0.0 && rotated >= 64.0 && rotated < 192.0) rev = 1.0;\n\n    // If the label should be invisible, we move the vertex outside\n    // of the view plane so that the triangle gets clipped. This makes it easier\n    // for us to create degenerate triangle strips.\n    // u_zoom is the current zoom level adjusted for the change in font size\n    float z = 2.0 - step(a_zoom[0], u_zoom) - (1.0 - step(a_zoom[1], u_zoom)) + rev;\n\n    // fade out labels\n    float alpha = clamp((u_fadezoom - a_labelminzoom) / u_fadedist, 0.0, 1.0);\n\n    if (u_fadedist >= 0.0) {\n        v_alpha = alpha;\n    } else {\n        v_alpha = 1.0 - alpha;\n    }\n    if (u_maxfadezoom < a_labelminzoom) {\n        v_alpha = 0.0;\n    }\n    if (u_minfadezoom >= a_labelminzoom) {\n        v_alpha = 1.0;\n    }\n\n    // if label has been faded out, clip it\n    z += step(v_alpha, 0.0);\n\n    // all the angles are 0..256 representing 0..2PI\n    // hide if (angle >= a_rangeend && angle < rangestart)\n    z += step(a_range[0], u_angle) * (1.0 - step(a_range[1], u_angle));\n\n    gl_Position = u_matrix * vec4(a_pos, 0, 1) + u_exmatrix * vec4(a_offset / 64.0, z, 0);\n    v_tex = a_tex * 4.0 / u_texsize;\n}\n",
       "precision highp float;\nuniform sampler2D u_texture;\nuniform vec4 u_color;\nuniform float u_buffer;\nuniform float u_gamma;\n\nvarying vec2 v_tex;\nvarying float v_alpha;\n\nvoid main() {\n    float dist = texture2D(u_texture, v_tex).a;\n    float alpha = smoothstep(u_buffer - u_gamma, u_buffer + u_gamma, dist) * v_alpha;\n    gl_FragColor = u_color * alpha;\n}\n",
   }
};
#endif
