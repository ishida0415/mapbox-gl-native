attribute vec2 a_pos;
uniform mat4 u_matrix;
uniform vec2 u_world;

varying vec2 v_pos;

void main() {
    // If the x coordinate is the maximum integer, we move the z coordinates out
    // of the view plane so that the triangle gets clipped. This makes it easier
    // for us to create degenerate triangle strips.
    gl_Position = u_matrix * vec4(a_pos, 0, 1);

    v_pos = (gl_Position.xy + 1.0) / 2.0 * u_world;
}
