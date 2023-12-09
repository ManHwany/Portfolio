using AppleAuth;
using AppleAuth.Enums;
using AppleAuth.Interfaces;
using AppleAuth.Native;
using System;

/**
 * Apple Login Manager
 * */

public class AppleLoginManager : SingletonMonobehavior<AppleLoginManager>
{
    private IAppleAuthManager _appleAuthManager;

    public override void Initialize()
    {
        base.Initialize();

        // If the current platform is supported
        if (AppleAuthManager.IsCurrentPlatformSupported)
        {
            // Creates a default JSON deserializer, to transform JSON Native responses to C# instances
            var deserializer = new PayloadDeserializer();
            // Creates an Apple Authentication manager with the deserializer
            _appleAuthManager = new AppleAuthManager(deserializer);
        }
    }

    public void SignInWithApple(Action<ICredential> successCallback, Action<IAppleError> errorCallback)
    {
        if (_appleAuthManager == null)
        {
            return;
        }

        var loginArgs = new AppleAuthLoginArgs(LoginOptions.IncludeEmail | LoginOptions.IncludeFullName);

        _appleAuthManager.LoginWithAppleId(loginArgs, successCallback, errorCallback);
    }

    private void Update()
    {
        if (_appleAuthManager == null)
        {
            return;
        }

        _appleAuthManager.Update();
    }
}