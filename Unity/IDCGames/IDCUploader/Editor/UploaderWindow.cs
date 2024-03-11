using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Runtime.Remoting.Lifetime;
using Unity.EditorCoroutines.Editor;
using UnityEditor;
using UnityEngine;
using UnityEngine.UIElements;
using static IDCConfig;
using static IDCConfig.ReleasesData;


public class UploaderWindow : EditorWindow {
    static IDCConfig IDCConfig;
    [MenuItem("Window/IDCGames/Build Uploader")]
    public static void ShowExample() {        
        UploaderWindow wnd = GetWindow<UploaderWindow>();
        wnd.titleContent = new GUIContent("Build Uploader");
    }

    Vector2 scrollPosition = new Vector2 (0, 0);
    bool isCalling = false;
    private void OnGUI() {
        IDCConfig.Get();
        if (IDCConfig.instance!=null) {
            string tmpString;
            GUILayout.Space(10);

            GUILayout.BeginHorizontal();
            GUILayout.Label("email: ", GUILayout.Width(100));
            tmpString = GUILayout.TextField(IDCConfig.instance.email);
            if (tmpString != IDCConfig.instance.email) {
                IDCConfig.instance.email = tmpString;
                EditorUtility.SetDirty(IDCConfig.instance);
            }
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            GUILayout.Label("GameID: ", GUILayout.Width(100));
            tmpString = GUILayout.TextField(IDCConfig.instance.GameID);
            if (tmpString != IDCConfig.instance.GameID) {
                IDCConfig.instance.GameID = tmpString;
                EditorUtility.SetDirty(IDCConfig.instance);
            }
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            GUILayout.Label("GameSecret: ", GUILayout.Width(100));
            tmpString = GUILayout.TextField(IDCConfig.instance.GameSecret);
            if (tmpString != IDCConfig.instance.GameSecret) {
                IDCConfig.instance.GameSecret = tmpString;
                EditorUtility.SetDirty(IDCConfig.instance);
            }
            GUILayout.EndHorizontal();
            GUILayout.BeginHorizontal();
            GUILayout.Label("");
            if (GUILayout.Button("Refresh Releases", GUILayout.Width(200)))
                IDCConfig.GetReleases((d) => { EditorUtility.SetDirty(IDCConfig.instance); });
            GUILayout.EndHorizontal();

            EditorGUILayout.LabelField("", GUI.skin.horizontalSlider);

            if (IDCConfig.instance.releasesData != null && IDCConfig.instance.releasesData.data!=null) {

                GUILayout.BeginHorizontal();
                GUILayout.Label("Releases: ");
                if (GUILayout.Button("Create New", GUILayout.Width(100))) {
                    string path = EditorUserBuildSettings.GetBuildLocation(EditorUserBuildSettings.activeBuildTarget).Replace("\\", "/");
                    int offset = path.LastIndexOf("/");

                    IDCConfig.CreateNewRelease($"New-Version-{Application.version}", $"\"{path.Substring(offset)}\"", (d) => {
                        var list = new List<ReleaseData>(IDCConfig.instance.releasesData.data);
                        list.Add(d.data);
                        IDCConfig.instance.releasesData.data = list.ToArray();
                    });
                }
                GUILayout.EndHorizontal();

                GUILayout.BeginHorizontal();
                GUILayout.Space(10);

                scrollPosition = GUILayout.BeginScrollView(scrollPosition);
                for (int i = 0; i < IDCConfig.instance.releasesData.data.Length; ++i) {
                    DrawItem(IDCConfig.instance.releasesData.data[i]);
                    if (i!= IDCConfig.instance.releasesData.data.Length-1 ) EditorGUILayout.LabelField("", GUI.skin.horizontalSlider);
                }
                GUILayout.EndScrollView();
                GUILayout.EndHorizontal();
            }
        }      
    }

    private void DrawItem(ReleaseData item) {
        string tmpString;
        GUILayout.BeginVertical();
        /*
        var texture = item.GetIconTexture();
        if (texture != null) {
            float aspect = texture.height / (float)texture.width;
            GUILayout.Box(item.GetIconTexture(), GUILayout.Width(128), GUILayout.Height(128 * aspect));
        }
        */
        GUILayout.BeginHorizontal();
        GUILayout.Label("ID: ", GUILayout.Width(100));
        GUILayout.Label(item.id.ToString());

        if (item.needUpdate) {
            if (GUILayout.Button("Update", GUILayout.Width(100))) {
                IDCConfig.UpdateRelease(item.id, item.name, item.exe, (d) => {
                    item.needUpdate = false;
                });

            }
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal();
        GUILayout.Label("Name: ", GUILayout.Width(100));
        tmpString = GUILayout.TextField(item.name);
        if (tmpString != item.name) {
            item.name = tmpString;
            item.needUpdate = true;
            EditorUtility.SetDirty(IDCConfig.instance);
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal();
        GUILayout.Label("Exe: ", GUILayout.Width(100));
        tmpString = GUILayout.TextField(item.exe);
        if (tmpString != item.exe) {
            item.exe = tmpString;
            item.needUpdate = true;
            EditorUtility.SetDirty(IDCConfig.instance);
        }
        GUILayout.EndHorizontal();

        GUILayout.BeginHorizontal();
        string publishedState = "Unknow.";
        switch (item.sync_status) {
            case 0:
                publishedState = "Unpublished.";
                break;
            case 1:
                publishedState = "Published:";
                break;
            case 2:
                publishedState = "Publishing...";
                break;

        }
        GUILayout.Label(publishedState, GUILayout.Width(100));
        DateTime publishedData;
        if (item.sync_status == 1) {
            if (DateTime.TryParse(item.published, out publishedData))
                GUILayout.Label(publishedData.ToString());
        } else {
            // Arrancar corrutina para comprobar el estado de publicacion.
            if (item.sync_status == 2) {
                if (item.checkPublishingCoroutine != null) EditorCoroutineUtility.StopCoroutine(item.checkPublishingCoroutine);
                item.checkPublishingCoroutine = EditorCoroutineUtility.StartCoroutine(CheckPublishingStatus(item), this);
            }
        }
        GUILayout.EndHorizontal();

        if (item.sync_status == 1) {
            GUILayout.BeginHorizontal();
            GUILayout.Label("Size: ", GUILayout.Width(100));
            GUILayout.Label(item.size);
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            GUILayout.Label("Active: ", GUILayout.Width(100));
            GUILayout.Label(item.active ? "Yes" : "No");
            if (GUILayout.Button(item.active ? "Deactivate" : "Activate", GUILayout.Width(100))) {
                IDCConfig.ActivateRelease(item.id, !item.active, (r) => {
                    if (r.success)  IDCConfig.GetReleases((d) => { EditorUtility.SetDirty(IDCConfig.instance); Repaint(); });
                    
                });
            }
            GUILayout.EndHorizontal();
        }
        if (string.IsNullOrEmpty(item.accessUntil)) {
            if (GUILayout.Button("Get FTP Access")) {
                IDCConfig.GetFTPAccess(item.id, (response2) => {
                    item.accessUntil = response2.accessUntil.ToString();
                    EditorUtility.SetDirty(IDCConfig.instance);
                });
            }
        } else {
            GUILayout.BeginHorizontal();
            GUILayout.Label("Limit: ", GUILayout.Width(100));
            GUILayout.Label(item.accessUntil.ToString());
            GUILayout.EndHorizontal();

            GUILayout.Space(10);
            GUILayout.Label("Check email for FTP access approved and copy User and Password.");

            GUILayout.BeginHorizontal();
            GUILayout.Label("User: ", GUILayout.Width(100));
            tmpString = GUILayout.TextField(item.ftpUser);
            if (tmpString != item.ftpUser) {
                item.ftpUser = tmpString;
                EditorUtility.SetDirty(IDCConfig.instance);
            }
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            GUILayout.Label("Pass: ", GUILayout.Width(100));
            tmpString = GUILayout.TextField(item.ftpPass);
            if (tmpString != item.ftpPass) {
                item.ftpPass = tmpString;
                EditorUtility.SetDirty(IDCConfig.instance);
            }
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal();
            GUILayout.Label("Last Build: ", GUILayout.Width(100));
            string path = EditorUserBuildSettings.GetBuildLocation(EditorUserBuildSettings.activeBuildTarget).Replace("\\", "/");
            if (!string.IsNullOrEmpty(path))
            {
                DateTime modification = File.GetLastWriteTime(path);
                GUILayout.Label(modification.ToString());
            }
            GUILayout.EndHorizontal();

            GUI.enabled = item.ftpPass.Length > 6 && item.ftpUser.Length > 6;
            if (GUILayout.Button("Upload Build")) {
                UploadBuild(item, path);
            }
            GUI.enabled = true;

        }
        GUILayout.EndVertical();
    }
    void UploadBuild(ReleaseData item, string path) {
        string url = "ftp://admin.idcgames.com";
        NetworkCredential credentials = new NetworkCredential(item.ftpUser, item.ftpPass);
        int offset = path.LastIndexOf("/");
        string[] directories = Directory.GetDirectories(path.Substring(0, offset), "*.*", SearchOption.AllDirectories);
        string[] files = Directory.GetFiles(path.Substring(0, offset), "*.*", SearchOption.AllDirectories);
        offset++;
        int idx = 0;
        
        foreach (var directory in directories) {
            EditorUtility.DisplayProgressBar("Uploading build", "Creating directory structure", idx / (float)directories.Length);
            var cleanDirectory = directory.Replace("\\", "/").Substring(offset);
            var request = WebRequest.Create($"{url}/{cleanDirectory}");
            request.Method = WebRequestMethods.Ftp.MakeDirectory;
            request.Credentials = credentials;
            try {
                FtpWebResponse resp = (FtpWebResponse)request.GetResponse();
            } catch(WebException we) {
                if ( ((FtpWebResponse)we.Response).StatusCode != FtpStatusCode.ActionNotTakenFileUnavailable) {
                    EditorUtility.ClearProgressBar();
                    // Show error!!!
                }
            }            
            idx++;
        }
        EditorUtility.ClearProgressBar();

        idx = 0;
        using (var client = new WebClient()) {
            client.Credentials = credentials;
            foreach (var file in files) {
                if (EditorUtility.DisplayCancelableProgressBar("Uploading build", $"File: {file.Substring(offset)}", idx / (float)files.Length))
                    break;
                var result = client.UploadFile($"{url}/{file.Substring(offset)}", WebRequestMethods.Ftp.UploadFile, file);                
                if (result.Length > 0) {
                    // Show error!!!
                    Debug.Log($"{System.Text.Encoding.Default.GetString(result)}");
                }
                idx++;
            }
        }
        EditorUtility.ClearProgressBar();
        IDCConfig.PublishRelease(item.id, (r) => {
            if (r.success) {
                item.sync_status = 2;
                if (item.checkPublishingCoroutine != null) EditorCoroutineUtility.StopCoroutine(item.checkPublishingCoroutine);
                item.checkPublishingCoroutine = EditorCoroutineUtility.StartCoroutine(CheckPublishingStatus(item), this);
                // Arrancar corrutina para comprobar el estado de publicacion.
            }
        });
    }

    static double nextTimeToCheck=0;
    IEnumerator CheckPublishingStatus(ReleaseData item) {
        while (item.sync_status==2) {
            if (EditorApplication.timeSinceStartup > nextTimeToCheck) {
                nextTimeToCheck = EditorApplication.timeSinceStartup + 5;
                int status = -1;
                yield return IDCConfig.CheckPublishStatus(item.id, (r) => { status = r.status; });
                item.sync_status = status;
                if (item.sync_status == 1) {
                    IDCConfig.GetReleases((d) => { EditorUtility.SetDirty(IDCConfig.instance); Repaint(); });
                }
            } else
                yield return null;
        }
    }
}
