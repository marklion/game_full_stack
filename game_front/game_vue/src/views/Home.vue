<template>
  <div class="home">
    <el-button v-if="!is_login" type="success" round @click="nav_to_wechat_login">
      微信登陆
    </el-button>
    <div v-else>
      <el-avatar :src="user_logo"></el-avatar>
      <div>{{user_name}}</div>
      <i class="el-icon-money">{{user_cash}}</i>
      <div><el-button type="danger" @click="logoff">退出登录</el-button></div>
    </div>
  </div>
</template>

<script>
// @ is an alias to /src
// import HelloWorld from '@/components/HelloWorld.vue'
import {Base64} from 'js-base64'

export default {
  name: 'Home',
  data: function() {
    return {
      user_name: '',
      user_logo: '',
      user_cash: '',
      is_login: false
    };
  },
  methods: {
    nav_to_wechat_login: function() {
      window.location.href = "https://open.weixin.qq.com/connect/qrconnect?appid=wx987b51617d4be3ae&redirect_uri=http%3a%2f%2fwww.d8sis.cn&response_type=code&scope=snsapi_login&state=STATE#wechat_redirect";
    },
    get_cur_user_info: function() {
      var vue_this = this;
      var ssid = this.$cookies.get('ssid');
      this.$axios.get('/game_rest/userinfo/' + ssid).then(function(resp) {
        if (resp.data.result.user_name.length > 0)
        {
          Base64.extendString();
          vue_this.user_name = resp.data.result.user_name.fromBase64();
          vue_this.user_logo = "data:image/jpg;base64," + resp.data.result.user_logo;
          vue_this.user_cash = resp.data.result.user_cash;
          vue_this.is_login = true;
        }
        console.log(vue_this);
      }).catch(function(err) {
        console.log(err);
      });
    },
    logoff: function() {
      var ssid = this.$cookies.get('ssid');
      this.$axios.get('/game_rest/logoff/' + ssid).then(function(resp) {
        console.log(resp);
        window.location.replace('/');
      }).catch(function(err) {
        console.log(err);
      });
    },
  },
  beforeMount: function() {
    var vue_this = this;
    this.get_cur_user_info();
    if (this.$route.query.code)
    {
      this.axios.post('/game_rest/login', {code:this.$route.query.code}).then(
        function(resp) {
          vue_this.$cookies.set("ssid", resp.data.result);
          window.location.replace('/');
        }
      ).catch(function(err) {
        console.log(err);
      });
    }
  }
}
</script>
