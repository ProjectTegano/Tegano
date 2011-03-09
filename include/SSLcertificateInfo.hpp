#ifndef _SSL_CERTIFICATE_INFO_HPP_INCLUDED
#define _SSL_CERTIFICATE_INFO_HPP_INCLUDED


#include <string>
#include <ctime>
#include <openssl/x509.h>

namespace _Wolframe {
	namespace Network {

		class SSLcertificateInfo
		{
		public:
			SSLcertificateInfo( X509* cert );

			long serialNumber() const		{ return serialNumber_; }
			const std::string& issuer() const	{ return issuer_; }
			time_t notBefore() const		{ return notBefore_; }
			time_t notAfter() const			{ return notAfter_; }
			const std::string& subject() const	{ return subject_; }
			const std::string& commonName() const	{ return commonName_; }
		private:
			long serialNumber_;
			std::string issuer_;
			time_t notBefore_;
			time_t notAfter_;
			std::string subject_;
			std::string commonName_;
		};

	} // namespace Network
} // namespace _Wolframe

#endif // _SSL_CERTIFICATE_INFO_HPP_INCLUDED
