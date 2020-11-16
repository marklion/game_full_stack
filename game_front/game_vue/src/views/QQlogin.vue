<template>
<div></div>
</template>

<script>
export default {
    name: 'QQlogin',
    beforeMount: function () {
        var vue_this = this;
        if (QC.Login.check()) {
            console.log("已经登录");
            QC.Login.getMe(function (openId, accessToken) { //该处的openId，accessToken就是后台需要的参数了，后台通过这些参数拿取临时登录凭证，然后就是自己的逻辑了
                console.log("opid:" + openId);
                console.log("acctok:" + accessToken);
                vue_this.$axios.post('http://www.d8sis.cn/game_rest/qq_login', {
                    text: {
                        openid: openId,
                        acctok: accessToken
                    }
                }).then(function (resp) {
                    vue_this.$cookies.set("ssid", resp.data.result);
                    window.location.replace('/');
                }).catch(function (err) {
                    console.log(err);
                });
            });
        } else {
            console.log("登录失败");
        }
    }
}
</script>

<style>

</style>
