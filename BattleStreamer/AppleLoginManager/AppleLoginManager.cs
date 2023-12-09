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
	
	/**
	*실 사용 예
	*/
	private void Example()
	{
		  AppleLoginManager.i.SignInWithApple(
          (credential) =>
          {
              // apple sign success
              resSocialType = (byte)socialType;
              resSnsId = credential.User;
			  
			  // 서버 Req
              ReqGlobalBindSnsID(resSocialType, resSnsId);
              DebugManager.Log("Apple Login Success!!! : " + credential.User);
          },
          (error) =>
          {              
              DebugManager.LogError("Sign in with Apple failed ");
          });
	}	

	private async void ReqGlobalBindSnsID(byte socialType, string snsID)
	{
		var (hasError, ack) = await WebAPI.SendReqAsync<diverseshared.ReqGlobalBindSnsID, diverseshared.AckGlobalBindSnsID>("/user/globalbindsnsid",
		new diverseshared.ReqGlobalBindSnsID
		{
			FpID = GamePlayData.i.UserAccount.Fpid,
			SocialType = socialType,
			SnsID = snsID,
		});
	
		if (hasError || ack.RetCode != diverseshared.ERROR_CODE_SPEC.Success)
		{
			PopupManager.i.ShowPopup_Toast(ack.RetMessage);
			return;
		}
	
		//1이 오면 요청 패킷 날리기 (0이 오면 이미 받은 것)
		if (ack.CanReceiveSnsReward == 1)
		{
			var (err, ackAfter) = await NetworkHubManager.i.BlockingSendAsync<AckAfterSnsBind>("ReqAfterSnsBind");
			if (err == RESPONSE_ERR.None)
			{
				if (GamePlayData.i.UserAccount.SNSRewardReceived == false)
				{
					GamePlayData.i.UserAccount.SNSRewardReceived = true;
					snsRewardGo.SetActive(false);
	
					NoticeManager.i.SetCondition(
						GameDB.ENoticeCondition.Cond_Option_SNS_Sync,
						!GamePlayData.i.UserAccount.SNSRewardReceived,
						GameDB.ENoticeCategory.Cat_Option_SNS_Btn, GameDB.ENoticeCategory.Cat_Option_Account_Tab, GameDB.ENoticeCategory.Cat_Option_Btn,
						GameDB.ENoticeCategory.Cat_TopBattleMenu_Btn);
				}
			}
			else
			{
				DebugManager.LogError(string.Format("'<color=green>ReqAfterSnsBind</color>' error: '<color=red>{0}</color>'", err));
			}
		}
	
		var socialTypeEnum = (Global.ESocialType)socialType;
		if (GamePlayData.i.UserAccount.AddSocialAccount(socialTypeEnum, snsID))
		{
			string contentString = MultiLanguageManager.i.GetText("Account_UI_Desc34");
			var compMsg = string.Format("{0} " + contentString, socialTypeEnum.ToString());
			PopupManager.i.ShowPopup_Toast(compMsg);
	
			//Refresh
			ShowSettingAccount();
		}
	}	
}