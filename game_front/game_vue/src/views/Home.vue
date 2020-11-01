<template>
  <div class="home">
    主页
    <el-button type="success" round @click="nav_to_wechat_login">
      微信登陆
    </el-button>
  </div>
</template>

<script>
// @ is an alias to /src
// import HelloWorld from '@/components/HelloWorld.vue'

export default {
  name: 'Home',
  methods: {
    nav_to_wechat_login: function() {
      window.location.href = "https://open.weixin.qq.com/connect/qrconnect?appid=wx987b51617d4be3ae&redirect_uri=http%3a%2f%2fwww.d8sis.cn&response_type=code&scope=snsapi_login&state=STATE#wechat_redirect";
    }
  },
  beforeMount: function() {
    var vue_this = this;
    this.axios.post('http://localhost/game_rest/login', {code:this.$route.query.code}).then(
      function(resp) {
        vue_this.$cookies.set("ssid", resp.data.result);
      }
    ).catch(function(err) {
      console.log(err);
    });
  }
}
</script>
