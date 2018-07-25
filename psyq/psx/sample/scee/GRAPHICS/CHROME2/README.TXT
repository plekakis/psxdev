INSTRUCTIONS: To run, type LOAD_IT.bat.  Then "run main.cpe".


MESSAGE FROM THE AUTHOR:

Hi developers,
here's a little chrome mapping demo for you.
It's not perfect but can give a nice effect when you pick
a nicer model and texture to use.
The vertex normals for the object are stored in an array
which is indexed by an integer stored along with the vertex.

the two main procedures are calc_chrome_uvs and change_texture.

calc_chrome_uvs will simply run through the array of normals
and apply the objects rotation matrix to each. Then the values are
scaled down to give the u v vals.

change_texture just looks up the values calculated by calc_chrome_textures
and applys the uv values to each of the polys.

Don't bother to look at tmd_shit.c as this is just a hack to convert the 
flat model to a textured Gouraud one.

You should be able to get this working even faster, but I'll let you work
that out for yourself, and if you get a good graphics book you'll see
that this can be used for a basic enviroment mapped demo i.e. stick a sphere
in the middle of the room and get you high power SG to enviroment map it, then
turn the sphere into a flat texture and hey presto a basic enviroment map demo.

Type psymake load	to load the model and texture
then run main.cpe

you can e-mail me at David_Virapen@interactive.sony.com

now added support for 3 different models with different textures,
plus a mad semitrans rainbow thing and stupid looking Gouraud background
(I was bored)
Can also cope with multiple textures and it now doesn't matter where in the
vram the texture is located. The texture used must be 64/64 8-bit but I might
update it again to deal with different sized textures plus support for both
4 and 8 bit.
