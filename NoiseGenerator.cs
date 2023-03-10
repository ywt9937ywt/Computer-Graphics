using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NoiseGenerator : MonoBehaviour
{
    public int tDivisions;
    public float tSize;
    public float tHeight;
    private static readonly Vector2[] gradient_pool = new Vector2[8]{new Vector2(1.0f, 1.0f), new Vector2(-1.0f,1.0f), new Vector2(1.0f,-1.0f),new Vector2(-1.0f,-1.0f),new Vector2(1.0f, 0.0f),
        new Vector2(-1.0f, 0.0f), new Vector2(0.0f, 1.0f), new Vector2(0.0f, -1.0f)};
    private Vector2[,] v_gradient = new Vector2[130, 130];
    // Start is called before the first frame update
    void Start()
    {
        for (int i = 0; i <= tDivisions; i++)
        {
            for (int j = 0; j <= tDivisions; j++)
            {
                var rnd = new System.Random();
                int rand_index = rnd.Next(0,7);
                v_gradient[i,j] = gradient_pool[rand_index];
            }
        }
        CreateTerrain();
        //diamond_square();
    }

    void CreateTerrain()
    {
        Texture2D texture = new Texture2D(128, 128);
        this.GetComponent<Renderer>().material.mainTexture = texture;
        int divP1 = tDivisions + 1;
        int vertCount = divP1 * divP1;
        Vector3[] verts = new Vector3[vertCount];
        Vector2[] uvs = new Vector2[vertCount];
        int[] tris = new int[tDivisions * tDivisions * 6];

        float halfSize = tSize * 0.5f;
        float divisionSize = tSize / tDivisions;

        Mesh mesh = new Mesh();
        GetComponent<MeshFilter>().mesh = mesh;

        int triOffset = 0;

        for(int i = 0; i <= tDivisions; i++)
        {
            for(int j = 0; j<= tDivisions; j++)
            {
                float xCoord = (float)j / tDivisions;
                float yCoord = (float)i / tDivisions;
                var rnd = new System.Random();
                float rand_index1 = rnd.Next(0, 99)* 0.01f;
                float rand_index2 = rnd.Next(0, 99)* 0.01f;
                float height = perlinNoise(i + rand_index1, j + rand_index2);
                //float height = Mathf.PerlinNoise(xCoord, yCoord);

                texture.SetPixel(i, j, new Color(height, height, height));

                verts[i * divP1 + j] = new Vector3(-halfSize + j * divisionSize, 0.5f * height, halfSize - i * divisionSize);
                uvs[i * divP1 + j] = new Vector2((float)j / tDivisions, (float)i / tDivisions);
                if(i < tDivisions && j < tDivisions)
                {
                    int topleft = i * divP1 + j;
                    int botleft = (i + 1) * divP1 + j;

                    tris[triOffset] = topleft;
                    tris[triOffset + 1] = topleft + 1;
                    tris[triOffset + 2] = botleft + 1;

                    tris[triOffset + 3] = topleft;
                    tris[triOffset + 4] = botleft + 1;
                    tris[triOffset + 5] = botleft;

                    triOffset += 6;
                }
            }
        }
        saveTexture(texture, "tex");
        Debug.Log(triOffset);

        mesh.vertices = verts;
        mesh.uv = uvs;
        mesh.triangles = tris;

        mesh.RecalculateBounds();
        mesh.RecalculateNormals();
    }

    void saveTexture(Texture2D texture, string fileName)
    {
        var bytes = texture.EncodeToPNG();
        var file = System.IO.File.Create(Application.dataPath + "/" + fileName + ".png");
        var binary = new System.IO.BinaryWriter(file);
        binary.Write(bytes);
        file.Close();
    }

    float perlinNoise(float x, float y)
    {
        int X = (int)System.Math.Floor(x);
        int Y = (int)System.Math.Floor(y);

        float u = (float)(x - X);
        float v = (float)(y - Y);

        float[] topleft = new float[2]{ u, v };
        float[] topright = new float[2] { u - 1, v };
        float[] bottomleft =  new float[2]{ u, v - 1 };
        float[] bottomright = new float[2] { u - 1, v - 1 };

        float g00 = topleft[0] * v_gradient[X,Y][0] + topleft[1] * v_gradient[X,Y][1];
        float g10 = topright[0] * v_gradient[X + 1,Y][0] + topright[1] * v_gradient[X + 1,Y][1];
        float g01 = bottomleft[0] * v_gradient[X,Y + 1][0] + bottomleft[1] * v_gradient[X,Y + 1][1];
        float g11 = bottomright[0] * v_gradient[X + 1,Y + 1][0] + bottomright[1] * v_gradient[X + 1,Y + 1][1];

        u = quinticFade(u);
        v = quinticFade(v);

        float xi1 = Lerp(g00, g10, u);
        float xi2 = Lerp(g01, g11, u);

        return Lerp(xi1, xi2, v);

    }

    void diamond_square()
    {
        int divP1 = tDivisions + 1;
        int vertCount = divP1 * divP1;
        float[] height = new float[vertCount];
        Vector3[] verts = new Vector3[vertCount];
        Vector2[] uvs = new Vector2[vertCount];
        int[] tris = new int[tDivisions * tDivisions * 6];

        var rnd = new System.Random();
        height[0] = rnd.Next(0, 99) * 0.01f;
        height[tDivisions] = rnd.Next(0, 99) * 0.01f;
        height[divP1 * tDivisions - 1] = rnd.Next(0, 99) * 0.01f;
        height[divP1 * divP1-1] = rnd.Next(0, 99) * 0.01f;

        float r = rnd.Next(0, 99) * 0.01f;
        int step_size = tDivisions;
        while(step_size > 1)
        {
            int halfSide = step_size / 2;
            for(int i = 0; i < tDivisions; i += step_size)
            {
                for (int j = 0; j < tDivisions; j += step_size)
                {
                    float diamondSum = height[i * divP1 + j] + height[(i + step_size) * divP1 + j] +
                                        height[i * divP1 + j + step_size] + height[(i + step_size) * divP1 + j + step_size];
                    float avg = diamondSum / 4;
                    avg += rnd.Next(0, 99) * 0.01f;
                    height[(i + halfSide) * divP1 + j + halfSide] = avg;
                }
            }

            for (int i = 0; i < tDivisions; i += halfSide)
            {
                for (int j = ((i+1)%2) * halfSide; j < tDivisions; j += step_size)
                {
                    int count = 0;
                    float squareSum = 0;
                    if (j - halfSide >= 0) // left point
                    {
                        squareSum += height[i * divP1 + j - halfSide];
                        count++;
                    }
                    if (i-halfSide >= 0)  // top
                    {
                        squareSum += height[(i - halfSide) * divP1 + j];
                        count++;
                    }
                    if (j + halfSide <= tDivisions) // right
                    {
                        squareSum += height[i * divP1 + j + halfSide];
                        count++;
                    }
                    if (i + halfSide <= tDivisions) // right
                    {
                        squareSum += height[(i + halfSide) * divP1 + j];
                        count++;
                    }
                    float avg = squareSum / count;
                    avg += rnd.Next(0, 99) * 0.01f;
                    height[i * divP1 + j ] = avg;
                }
            }
            step_size /= 2;
        }

        float halfSize = tSize * 0.5f;
        float divisionSize = tSize / tDivisions;
        int triOffset = 0;
        for (int i = 0; i <= tDivisions; i++)
        {
            for (int j = 0; j <= tDivisions; j++)
            {
                //texture.SetPixel(i, j, new Color(height, height, height));

                verts[i * divP1 + j] = new Vector3(-halfSize + j * divisionSize, 0.5f * height[i * divP1 + j], halfSize - i * divisionSize);
                uvs[i * divP1 + j] = new Vector2((float)j / tDivisions, (float)i / tDivisions);
                if (i < tDivisions && j < tDivisions)
                {
                    int topleft = i * divP1 + j;
                    int botleft = (i + 1) * divP1 + j;

                    tris[triOffset] = topleft;
                    tris[triOffset + 1] = topleft + 1;
                    tris[triOffset + 2] = botleft + 1;

                    tris[triOffset + 3] = topleft;
                    tris[triOffset + 4] = botleft + 1;
                    tris[triOffset + 5] = botleft;

                    triOffset += 6;
                }
            }
        }
        Mesh mesh = new Mesh();
        GetComponent<MeshFilter>().mesh = mesh;
        mesh.vertices = verts;
        mesh.uv = uvs;
        mesh.triangles = tris;

        mesh.RecalculateBounds();
        mesh.RecalculateNormals();
    }

    float quinticFade(float t)
    {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    float Lerp(float a, float b, float t) { return a + t * (b - a); }

    // Update is called once per frame
    void Update()
    {
        
    }
}
