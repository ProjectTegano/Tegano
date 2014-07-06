﻿using System.Security.Cryptography;

namespace WolframeClient
{
	class PasswordChangeMessage
	{
		private byte[]	message = new byte[64];

		public PasswordChangeMessage( string oldPassword, string challenge, string newPassword )
		{
			// process the challenge
			if ( challenge[0] != '$' )
				throw new System.ArgumentException( "Challenge does not begin with a \"$\"" );
			challenge.TrimStart( '$' );
			string[] parts = challenge.Split( '$' );
			if ( parts.Length != 3 )
				throw new System.ArgumentException( "Invalid challenge format" );
			byte[] salt = System.Convert.FromBase64String( parts[1] );
			byte[] chlng = System.Convert.FromBase64String( parts[2] );
			if ( chlng.Length != 64 )
				throw new System.ArgumentException( "Invalid challenge length" );
			Rfc2898DeriveBytes pwdHash = new Rfc2898DeriveBytes( oldPassword, salt, 10589 );
			byte[] passwd = pwdHash.GetBytes( 48 );

			byte[] response = new byte[64];
			for ( int i = 0; i < passwd.Length; i++ )
				response[i] = passwd[i];
			for ( int i = passwd.Length; i < 64; i++ )
				response[i] = 0x3c;
			for ( int i = 0; i < 64; i++ )
				response[i] ^= chlng[i];
			SHA256 sha = SHA256.Create();
			sha.ComputeHash( response );
			
			byte[] msg = new byte[48];
			for ( int i = 0; i < 48; i++ )
				msg[i] = 0;
			int pwLen = newPassword.Length > 47 ? 47 : newPassword.Length;
			for ( int i = 0; i < pwLen; i++ )
				msg[i + 1] = (byte)newPassword[i];
			msg[0] = (byte)pwLen;
			MD5 md5 = MD5.Create();
			md5.ComputeHash( msg );

			AesManaged myAes = new AesManaged();
			myAes.Mode = CipherMode.CBC;
			myAes.IV = salt;
			myAes.Key = sha.Hash;
			myAes.Padding = PaddingMode.None;
			ICryptoTransform encryptor = myAes.CreateEncryptor();
			encryptor.TransformBlock( msg, 0, 48, message, 0 );
			encryptor.TransformBlock( md5.Hash, 0, 16, message, 48 );
		}

		public string outString()
		{
			return System.Convert.ToBase64String( message );
		}
	}
}
