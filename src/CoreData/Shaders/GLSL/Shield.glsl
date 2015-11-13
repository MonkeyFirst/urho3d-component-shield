#include "Uniforms.glsl"
#include "Samplers.glsl"
#include "Transform.glsl"
#include "ScreenPos.glsl"
#include "Lighting.glsl"
#include "Fog.glsl"

// To PS
varying vec2 vTexCoord;
varying vec2 vTexCoord2;
varying vec4 vWorldPos;

#ifdef VERTEXCOLOR
    varying vec4 vColor;
#endif

#ifdef COMPILEPS
uniform float cPhase;
uniform float cHitPower;
#endif

void VS()
{
    mat4 modelMatrix = iModelMatrix;
    vec3 worldPos = GetWorldPos(modelMatrix);
    gl_Position = GetClipPos(worldPos);
    vTexCoord = GetTexCoord(iTexCoord);   // get final UV1
    vTexCoord2 = GetTexCoord(iTexCoord2); // get final UV2
    
    vWorldPos = vec4(worldPos, GetDepth(gl_Position));

    #ifdef VERTEXCOLOR
        vColor = iColor;
    #endif
}

void PS()
{
    // Get material diffuse albedo
    #ifdef DIFFMAP
        vec4 diffColor = cMatDiffColor * texture2D(sDiffMap, vTexCoord.xy);;
        vec4 shieldMask = texture2D(sNormalMap, vTexCoord2.xy);
        float maskIntensity = GetIntensity(shieldMask.rgb);
        maskIntensity = mix(maskIntensity, 0.0f, cPhase);
        float p = pow(maskIntensity, 4.0f) * cHitPower;
        diffColor = mix(vec4(0), diffColor, p);
        
        #ifdef ALPHAMASK
            if (diffColor.a < 0.5)
                discard;
        #endif
        
    #else
        vec4 diffColor = cMatDiffColor;
    #endif

    #ifdef VERTEXCOLOR
        diffColor *= vColor;
    #endif

    // Get fog factor
    #ifdef HEIGHTFOG
        float fogFactor = GetHeightFogFactor(vWorldPos.w, vWorldPos.y);
    #else
        float fogFactor = GetFogFactor(vWorldPos.w);
    #endif

    #if defined(PREPASS)
        // Fill light pre-pass G-Buffer
        gl_FragData[0] = vec4(0.5, 0.5, 0.5, 1.0);
        gl_FragData[1] = vec4(EncodeDepth(vWorldPos.w), 0.0);
    #elif defined(DEFERRED)
        gl_FragData[0] = vec4(GetFog(diffColor.rgb, fogFactor), diffColor.a);
        gl_FragData[1] = vec4(0.0, 0.0, 0.0, 0.0);
        gl_FragData[2] = vec4(0.5, 0.5, 0.5, 1.0);
        gl_FragData[3] = vec4(EncodeDepth(vWorldPos.w), 0.0);
    #else
        gl_FragColor = vec4(GetFog(diffColor.rgb, fogFactor), diffColor.a);
    #endif
}
