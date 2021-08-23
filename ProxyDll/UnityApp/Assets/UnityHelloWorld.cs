using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[ExecuteAlways]
public class UnityHelloWorld : MonoBehaviour
{
    public string Message = "UnityHelloWorld! ハローゆにてい。";

    void OnGUI()
    {
       GUI.Label(new Rect(10, 10, 500, 20), Message);
    }
}
