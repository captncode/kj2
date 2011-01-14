
uniform sampler2D tex;

void main()
{
  vec4 t0col = texture2D(tex,gl_TexCoord[0].xy);
  vec4 t1col = texture2D(tex,gl_TexCoord[1].xy);
  float a0 = t0col.a-t1col.a;
  a0 = clamp(a0,0.f,1.f);
  //gl_FragColor.rgb = a0 * t0col.rgb + t1col.a * t1col.rgb;
  gl_FragColor = mix(t0col,t1col,t1col.a);
  //gl_FragColor.a = t1col.a * t1col.rgb;
}
