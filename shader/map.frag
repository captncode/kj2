
uniform sampler2D tex;

void main()
{
  vec4 t0col = texture2D(tex,gl_TexCoord[0].xy);
  vec4 t1col = texture2D(tex,gl_TexCoord[1].xy);
  vec4 t2col = texture2D(tex,gl_TexCoord[2].xy);
  vec4 t3col = texture2D(tex,gl_TexCoord[3].xy);
  //float a0 = t0col.a-t1col.a;
  //a0 = clamp(a0,0.f,1.f);
  //gl_FragColor.rgb = a0 * t0col.rgb + t1col.a * t1col.rgb;
  vec4 outCol = mix(t0col,t1col,t1col.a);
  outCol = mix(outCol,t2col,t2col.a);
  outCol = mix(outCol,t3col,t3col.a);

  gl_FragColor = outCol;
}
