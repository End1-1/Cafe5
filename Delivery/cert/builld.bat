keytool -genkey -dname "cn=ssltest, ou=test, o=example, c=US" -alias ssltest -keypass ssltest -keystore ssltest.keystore -storepass ssltest -validity 18000 
keytool -export -alias ssltest -keystore ssltest.keystore -file ssltest.cer -storepass ssltest -keypass ssltest 
