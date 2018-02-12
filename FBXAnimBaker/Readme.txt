Here is the source code for the program to bake animations.
It takes two FBX as input. The first being the model in the bind pose, the second being the animation. You can also input the name you want the output to use (though it tacks on some stuff on the end).
It outputs the animation as a texture, and a modified version of the input model that has the vertices numbered using the vertex color channels.
Because of how the vertex indices are baked into the vertex color data there is a hard limit on how many vertices the model can have.
The code is dependent on Autodesk's FBXSDK and DevIL (Dev Image Library).
