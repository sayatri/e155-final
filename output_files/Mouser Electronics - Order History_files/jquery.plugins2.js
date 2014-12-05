(function(a){if(!a.browser){a.extend({browser:function(){var b={};try{navigator.vendor?/Chrome/.test(navigator.userAgent)?(b.browser="Chrome",b.version=parseFloat(navigator.userAgent.split("Chrome/")[1].split("Safari")[0])):/Safari/.test(navigator.userAgent)?(b.browser="Safari",b.version=parseFloat(navigator.userAgent.split("Version/")[1].split("Safari")[0])):/Opera/.test(navigator.userAgent)&&(b.Opera="Safari",b.version=parseFloat(navigator.userAgent.split("Version/")[1])):/Firefox/.test(navigator.userAgent)?(b.browser="Firefox",b.version=parseFloat(navigator.userAgent.split("Firefox/")[1])):(b.browser="MSIE",/MSIE/.test(navigator.userAgent)?b.version=parseFloat(navigator.userAgent.split("MSIE")[1]):b.version="edge")
}catch(c){b=c}return b}})}})(jQuery);jQuery.url=function(){var e={};var b={};var d={url:window.location,strictMode:false,key:["source","protocol","authority","userInfo","user","password","host","port","relative","path","directory","file","query","anchor"],q:{name:"queryKey",parser:/(?:^|&)([^&=]*)=?([^&]*)/g},parser:{strict:/^(?:([^:\/?#]+):)?(?:\/\/((?:(([^:@]*):?([^:@]*))?@)?([^:\/?#]*)(?::(\d*))?))?((((?:[^?#\/]*\/)*)([^?#]*))(?:\?([^#]*))?(?:#(.*))?)/,loose:/^(?:(?![^:@]+:[^:@\/]*@)([^:\/?#.]+):)?(?:\/\/)?((?:(([^:@]*):?([^:@]*))?@)?([^:\/?#]*)(?::(\d*))?)(((\/(?:[^?#](?![^?#\/]*\.[^?#\/.]+(?:[?#]|$)))*\/?)?([^?#\/]*))(?:\?([^#]*))?(?:#(.*))?)/}};
var c=function(){str=decodeURI(d.url);var j=d.parser[d.strictMode?"strict":"loose"].exec(str);var l={};var k=14;while(k--){l[d.key[k]]=j[k]||""}l[d.q.name]={};l[d.key[12]].replace(d.q.parser,function(m,i,n){if(i){l[d.q.name][i]=n}});return l};var f=function(i){if(!b.length){a()}if(i=="base"){if(b.port!==null&&b.port!==""){return b.protocol+"://"+b.host+":"+b.port+"/"}else{return b.protocol+"://"+b.host+"/"}}return(b[i]==="")?null:b[i]};var h=function(i){if(!b.length){a()}return(b.queryKey[i]===null)?null:b.queryKey[i]
};var a=function(){b=c();g()};var g=function(){var i=b.path;e=[];e=b.path.length==1?{}:(i.charAt(i.length-1)=="/"?i.substring(1,i.length-1):path=i.substring(1)).split("/")};return{setMode:function(i){strictMode=i=="strict"?true:false;return this},setUrl:function(i){d.url=i===undefined?window.location:i;a();return this},segment:function(i){if(!b.length){a()}if(i===undefined){return e.length}return(e[i]===""||e[i]===undefined)?null:e[i]},attr:f,param:h}}();
/* =============================================================
* jQuery Tooltip plugin 1.3
*
* http://bassistance.de/jquery-plugins/jquery-plugin-tooltip/
* http://docs.jquery.com/Plugins/Tooltip
*
* Copyright (c) 2006 - 2008 J�rn Zaefferer
*
* $Id: jquery.tooltip.js 5741 2008-06-21 15:22:16Z joern.zaefferer $
* 
* Dual licensed under the MIT and GPL licenses:
*   http://www.opensource.org/licenses/mit-license.php
*   http://www.gnu.org/licenses/gpl.html
*/
eval(function(h,b,i,d,g,f){g=function(a){return(a<b?"":g(parseInt(a/b)))+((a=a%b)>35?String.fromCharCode(a+29):a.toString(36))
};if(!"".replace(/^/,String)){while(i--){f[g(i)]=d[i]||g(i)}d=[function(a){return f[a]}];g=function(){return"\\w+"};i=1}while(i--){if(d[i]){h=h.replace(new RegExp("\\b"+g(i)+"\\b","g"),d[i])}}return h}(";(8($){j e={},9,m,B,A=$.2u.2g&&/29\\s(5\\.5|6\\.)/.1M(1H.2t),M=12;$.k={w:12,1h:{Z:25,r:12,1d:19,X:\"\",G:15,E:15,16:\"k\"},2s:8(){$.k.w=!$.k.w}};$.N.1v({k:8(a){a=$.1v({},$.k.1h,a);1q(a);g 2.F(8(){$.1j(2,\"k\",a);2.11=e.3.n(\"1g\");2.13=2.m;$(2).24(\"m\");2.22=\"\"}).21(1e).1U(q).1S(q)},H:A?8(){g 2.F(8(){j b=$(2).n('Y');4(b.1J(/^o\\([\"']?(.*\\.1I)[\"']?\\)$/i)){b=1F.$1;$(2).n({'Y':'1D','1B':\"2r:2q.2m.2l(2j=19, 2i=2h, 1p='\"+b+\"')\"}).F(8(){j a=$(2).n('1o');4(a!='2f'&&a!='1u')$(2).n('1o','1u')})}})}:8(){g 2},1l:A?8(){g 2.F(8(){$(2).n({'1B':'',Y:''})})}:8(){g 2},1x:8(){g 2.F(8(){$(2)[$(2).D()?\"l\":\"q\"]()})},o:8(){g 2.1k('28')||2.1k('1p')}});8 1q(a){4(e.3)g;e.3=$('<t 16=\"'+a.16+'\"><10></10><t 1i=\"f\"></t><t 1i=\"o\"></t></t>').27(K.f).q();4($.N.L)e.3.L();e.m=$('10',e.3);e.f=$('t.f',e.3);e.o=$('t.o',e.3)}8 7(a){g $.1j(a,\"k\")}8 1f(a){4(7(2).Z)B=26(l,7(2).Z);p l();M=!!7(2).M;$(K.f).23('W',u);u(a)}8 1e(){4($.k.w||2==9||(!2.13&&!7(2).U))g;9=2;m=2.13;4(7(2).U){e.m.q();j a=7(2).U.1Z(2);4(a.1Y||a.1V){e.f.1c().T(a)}p{e.f.D(a)}e.f.l()}p 4(7(2).18){j b=m.1T(7(2).18);e.m.D(b.1R()).l();e.f.1c();1Q(j i=0,R;(R=b[i]);i++){4(i>0)e.f.T(\"<1P/>\");e.f.T(R)}e.f.1x()}p{e.m.D(m).l();e.f.q()}4(7(2).1d&&$(2).o())e.o.D($(2).o().1O('1N://','')).l();p e.o.q();e.3.P(7(2).X);4(7(2).H)e.3.H();1f.1L(2,1K)}8 l(){B=S;4((!A||!$.N.L)&&7(9).r){4(e.3.I(\":17\"))e.3.Q().l().O(7(9).r,9.11);p e.3.I(':1a')?e.3.O(7(9).r,9.11):e.3.1G(7(9).r)}p{e.3.l()}u()}8 u(c){4($.k.w)g;4(c&&c.1W.1X==\"1E\"){g}4(!M&&e.3.I(\":1a\")){$(K.f).1b('W',u)}4(9==S){$(K.f).1b('W',u);g}e.3.V(\"z-14\").V(\"z-1A\");j b=e.3[0].1z;j a=e.3[0].1y;4(c){b=c.2o+7(9).E;a=c.2n+7(9).G;j d='1w';4(7(9).2k){d=$(C).1r()-b;b='1w'}e.3.n({E:b,14:d,G:a})}j v=z(),h=e.3[0];4(v.x+v.1s<h.1z+h.1n){b-=h.1n+20+7(9).E;e.3.n({E:b+'1C'}).P(\"z-14\")}4(v.y+v.1t<h.1y+h.1m){a-=h.1m+20+7(9).G;e.3.n({G:a+'1C'}).P(\"z-1A\")}}8 z(){g{x:$(C).2e(),y:$(C).2d(),1s:$(C).1r(),1t:$(C).2p()}}8 q(a){4($.k.w)g;4(B)2c(B);9=S;j b=7(2);8 J(){e.3.V(b.X).q().n(\"1g\",\"\")}4((!A||!$.N.L)&&b.r){4(e.3.I(':17'))e.3.Q().O(b.r,0,J);p e.3.Q().2b(b.r,J)}p J();4(7(2).H)e.3.1l()}})(2a);",62,155,"||this|parent|if|||settings|function|current||||||body|return|||var|tooltip|show|title|css|url|else|hide|fade||div|update||blocked|||viewport|IE|tID|window|html|left|each|top|fixPNG|is|complete|document|bgiframe|track|fn|fadeTo|addClass|stop|part|null|append|bodyHandler|removeClass|mousemove|extraClass|backgroundImage|delay|h3|tOpacity|false|tooltipText|right||id|animated|showBody|true|visible|unbind|empty|showURL|save|handle|opacity|defaults|class|data|attr|unfixPNG|offsetHeight|offsetWidth|position|src|createHelper|width|cx|cy|relative|extend|auto|hideWhenEmpty|offsetTop|offsetLeft|bottom|filter|px|none|OPTION|RegExp|fadeIn|navigator|png|match|arguments|apply|test|http|replace|br|for|shift|click|split|mouseout|jquery|target|tagName|nodeType|call||mouseover|alt|bind|removeAttr|200|setTimeout|appendTo|href|MSIE|jQuery|fadeOut|clearTimeout|scrollTop|scrollLeft|absolute|msie|crop|sizingMethod|enabled|positionLeft|AlphaImageLoader|Microsoft|pageY|pageX|height|DXImageTransform|progid|block|userAgent|browser".split("|"),0,{}))
/* =============================================================
* Thickbox 3.1 - One Box To Rule Them All.
* By Cody Lindley (http://www.codylindley.com)
* Copyright (c) 2007 cody lindley
* Licensed under the MIT License: http://www.opensource.org/licenses/mit-license.php
*/
;
var tb_pathToImage="/images/loadingAnimation.gif";
/*!!!!!!!!!!!!!!!! edit below this line at your own risk !!!!!!!!!!!!!!!!!!!!!!!*/
jQuery.ooCookie=function(CookieName){return{setCookie:function(options){var cookieContent="";options=options||{};var filterProperties={cookieID:0,setCookie:1,getCookie:2};for(var property in this){if(!(property in filterProperties)){if(this[property]===null){this[property]=""}cookieContent+=(property)+"="+(this[property])+"&"}}cookieContent=cookieContent.substring(0,cookieContent.length-1);
var expires="";if(options.expires&&(typeof options.expires=="number"||options.expires.toUTCString)){var date;if(typeof options.expires=="number"){date=new Date();date.setTime(date.getTime()+(options.expires*24*60*60*1000))}else{date=options.expires}expires="; expires="+date.toUTCString()}var path=options.path?"; path="+(options.path):"";var domain=options.domain?"; domain="+(options.domain):"";var secure=options.secure?"; secure":"";document.cookie=[this.cookieID,"=",cookieContent,expires,path,domain,secure].join("");
return true},getCookie:function(){var cookieValue=null;if(document.cookie&&document.cookie!=""){var cookies=document.cookie.split(";");for(var i=0;i<cookies.length;i++){var cookie=jQuery.trim(cookies[i]);if(cookie.substring(0,this.cookieID.length+1)==(this.cookieID+"=")){cookieValue=cookie.substring(this.cookieID.length+1);break}}var properties=cookieValue.split("&");for(var i=0;i<properties.length;i++){var property=properties[i].split("=");eval('this["'+property[0]+'"]="'+property[1]+'"')}}return true},cookieID:CookieName}
};$(document).ready(function(){tb_init("a.thickbox, area.thickbox, input.thickbox");imgLoader=new Image();imgLoader.src=tb_pathToImage});function tb_init(a){$(document).on("click",a,function(e){var c=this.title||this.name||null;var b=$(this).attr("href")||this.alt;var d=this.rel||false;if((e.button!=0)&&(e.button!=undefined)){return true}tb_show(c,b,d);this.blur();return false})}function tb_show(l,b,j){try{var d=b.replace(/^[^\?]+\??/,"");var f=tb_parseQuery(d);if(typeof document.body.style.maxHeight==="undefined"){$("body","html").css({height:"100%",width:"100%"});
$("html").css("overflow","hidden");if(document.getElementById("TB_HideSelect")===null){$("body form:first").append("<iframe id='TB_HideSelect'></iframe><div id='TB_overlay'></div><div id='TB_window'></div>");$("#TB_overlay").click(tb_remove)}}else{if(document.getElementById("TB_overlay")===null){$("body form:first").append("<div id='TB_overlay'></div><div id='TB_window'></div>");$("#TB_overlay").click(tb_remove);if($("#TB_title").length!=0){$("#TB_title").remove()}}}if(tb_detectMacXFF()){$("#TB_overlay").addClass("TB_overlayMacFFBGHack")
}else{$("#TB_overlay").addClass("TB_overlayBG")}if(l===null){l=""}$("body").append("<div id='TB_load'><img src='"+imgLoader.src+"' /></div>");$("#TB_load").show();var c;if(b.indexOf("?")!==-1){c=b.substr(0,b.indexOf("?"))}else{c=b}var i=/\.jpg$|\.jpeg$|\.png$|\.gif$|\.bmp$/;var n=c.toLowerCase().match(i);if(n==".jpg"||n==".jpeg"||n==".png"||n==".gif"||n==".bmp"){TB_PrevCaption="";TB_PrevURL="";TB_PrevHTML="";TB_NextCaption="";TB_NextURL="";TB_NextHTML="";TB_imageCount="";TB_FoundURL=false;if(j){TB_TempArray=$("a[@rel="+j+"]").get();
for(TB_Counter=0;((TB_Counter<TB_TempArray.length)&&(TB_NextHTML===""));TB_Counter++){var h=TB_TempArray[TB_Counter].href.toLowerCase().match(i);if(!(TB_TempArray[TB_Counter].href==b)){if(TB_FoundURL){TB_NextCaption=TB_TempArray[TB_Counter].title;TB_NextURL=TB_TempArray[TB_Counter].href;TB_NextHTML="<span id='TB_next'>&nbsp;&nbsp;<a href='#'>Next &gt;</a></span>"}else{TB_PrevCaption=TB_TempArray[TB_Counter].title;TB_PrevURL=TB_TempArray[TB_Counter].href;TB_PrevHTML="<span id='TB_prev'>&nbsp;&nbsp;<a href='#'>&lt; Prev</a></span>"
}}else{TB_FoundURL=true;TB_imageCount="Image "+(TB_Counter+1)+" of "+(TB_TempArray.length)}}}imgPreloader=new Image();imgPreloader.onload=function(){imgPreloader.onload=null;var q=tb_getPageSize();var o=q[0]-150;var t=q[1]-150;var p=imgPreloader.width;var e=imgPreloader.height;if(p>o){e=e*(o/p);p=o;if(e>t){p=p*(t/e);e=t}}else{if(e>t){p=p*(t/e);e=t;if(p>o){e=e*(o/p);p=o}}}TB_WIDTH=p+30;TB_HEIGHT=e+60;$("#TB_window").append("<a href='' id='TB_ImageOff' title='Close'><img id='TB_Image' src='"+b+"' width='"+p+"' height='"+e+"' alt='"+l+"'/></a><div id='TB_caption'>"+l+"<div id='TB_secondLine'>"+TB_imageCount+TB_PrevHTML+TB_NextHTML+"</div></div><div id='TB_closeWindow'><a href='#' id='TB_closeWindowButton' title='Close'>close</a> or Esc Key</div>");
$("#TB_closeWindowButton").click(tb_remove);if(!(TB_PrevHTML==="")){function s(){if($(document).unbind("click",s)){$(document).unbind("click",s)}$("#TB_window").remove();$("body").append("<div id='TB_window'></div>");tb_show(TB_PrevCaption,TB_PrevURL,j);return false}$("#TB_prev").click(s)}if(!(TB_NextHTML==="")){function r(){$("#TB_window").remove();$("body").append("<div id='TB_window'></div>");tb_show(TB_NextCaption,TB_NextURL,j);return false}$("#TB_next").click(r)}document.onkeydown=function(u){if(u==null){keycode=event.keyCode
}else{keycode=u.which}if(keycode==27){tb_remove()}else{if(keycode==190){if(!(TB_NextHTML=="")){document.onkeydown="";r()}}else{if(keycode==188){if(!(TB_PrevHTML=="")){document.onkeydown="";s()}}}}};tb_position();$("#TB_load").remove();$("#TB_ImageOff").click(tb_remove);$("#TB_window").css({display:"block"})};imgPreloader.src=b}else{var a=b.replace(/^[^\?]+\??/,"");var g=tb_parseQuery(a);TB_WIDTH=(g.width*1)+30||630;TB_HEIGHT=(g.height*1)+40||440;ajaxContentW=TB_WIDTH-30;ajaxContentH=TB_HEIGHT-45;if(b.indexOf("TB_iframe")!=-1){var m=f.QuickView=="true"?true:false;
urlNoQuery=b.split("TB_");$("#TB_iframeContent").remove();if(g.modal!="true"){$("#TB_window").append("<div id='TB_title'><div id='TB_ajaxWindowTitle'>"+l+"</div><div id='TB_closeAjaxWindow'><a href='#' id='TB_closeWindowButton' title='Close'>close</a> or Esc Key</div></div><iframe frameborder='0' hspace='0' src='"+urlNoQuery[0]+"' id='TB_iframeContent' name='TB_iframeContent"+Math.round(Math.random()*1000)+"' onload='tb_showIframe("+m+")' style='width:"+(ajaxContentW+29)+"px;height:"+(ajaxContentH+17)+"px;' > </iframe>")
}else{$("#TB_overlay").unbind();$("#TB_window").append("<iframe frameborder='0' hspace='0' src='"+urlNoQuery[0]+"' id='TB_iframeContent' name='TB_iframeContent"+Math.round(Math.random()*1000)+"' onload='tb_showIframe("+m+")' style='width:"+(ajaxContentW+29)+"px;height:"+(ajaxContentH+17)+"px;'> </iframe>")}}else{if($("#TB_window").css("display")!="block"){if(g.modal!="true"){$("#TB_window").append("<div id='TB_title'><div id='TB_ajaxWindowTitle'>"+l+"</div><div id='TB_closeAjaxWindow'><a href='#' id='TB_closeWindowButton'>close</a> or Esc Key</div></div><div id='TB_ajaxContent' style='width:"+ajaxContentW+"px;height:"+ajaxContentH+"px'></div>")
}else{$("#TB_overlay").unbind();$("#TB_window").append("<div id='TB_ajaxContent' class='TB_modal' style='width:"+ajaxContentW+"px;height:"+ajaxContentH+"px;'></div>")}}else{$("#TB_ajaxContent")[0].style.width=ajaxContentW+"px";$("#TB_ajaxContent")[0].style.height=ajaxContentH+"px";$("#TB_ajaxContent")[0].scrollTop=0;$("#TB_ajaxWindowTitle").html(l)}}$("#TB_closeWindowButton").click(tb_remove);if(b.indexOf("TB_inline")!=-1){$("#TB_ajaxContent").append($("#"+g.inlineId).children());$("#TB_window").unload(function(){$("#"+g.inlineId).append($("#TB_ajaxContent").children())
});tb_position();$("#TB_load").remove();$("#TB_window").css({display:"block"})}else{if(b.indexOf("TB_iframe")!=-1){tb_position();if($.browser.safari){$("#TB_load").remove();$("#TB_window").css({display:"block"})}}else{$("#TB_ajaxContent").load(b+="&random="+(new Date().getTime()),function(){tb_position();$("#TB_load").remove();tb_init("#TB_ajaxContent a.thickbox");$("#TB_window").css({display:"block"})})}}}if(!g.modal){document.onkeyup=function(o){if(o==null){keycode=event.keyCode}else{keycode=o.which}if(keycode==27){tb_remove()
}}}if(f.QuickView=="true"){$("#TB_window").draggable({handle:"#TB_window"}).addClass("quickview");$("#TB_overlay").remove();$("#TB_title").css({display:"block"});tb_absolute_pos()}}catch(k){}}function tb_showIframe(b){$("#TB_load").remove();$("#TB_window").css({display:"block"});var a=$("#TB_iframeContent").contents().find("#ProductDetailWrapper").height()+48+"px";var c=$("#TB_iframeContent").contents().find("#ProductDetailWrapper").height()+20+"px";if(b){$("#TB_window").css({height:a});$("#TB_iframeContent").css({height:c})
}}function tb_remove(){$("#TB_imageOff").unbind("click");$("#TB_closeWindowButton").unbind("click");$("#TB_window").fadeOut("fast",function(){if($(this).find("#TB_iframeContent").length>=1){$("#TB_window,#TB_overlay,#TB_HideSelect").unload("#TB_iframeContent").unbind().remove()}else{$("#TB_window,#TB_overlay,#TB_HideSelect").trigger("unload").unbind().remove()}});$("#TB_load").remove();if(typeof document.body.style.maxHeight=="undefined"){$("body","html").css({height:"auto",width:"auto"});$("html").css("overflow","")
}document.onkeydown="";document.onkeyup="";return false}function tb_position(){$("#TB_window").css({marginLeft:"-"+parseInt((TB_WIDTH/2),10)+"px",width:TB_WIDTH+"px"});if(!(jQuery.browser.msie&&jQuery.browser.version<7)){$("#TB_window").css({marginTop:"-"+parseInt((TB_HEIGHT/2),10)+"px"})}}function tb_absolute_pos(){var a=tb_getPageSize();var b=((a[1]-$("TB_window").outerHeight())/2)+$(window).scrollTop()+"px";$("#TB_window").css({top:b,left:"50%",position:"absolute"})}function tb_parseQuery(d){var e={};if(!d){return e
}var a=d.split(/[;&]/);for(var c=0;c<a.length;c++){var g=a[c].split("=");if(!g||g.length!=2){continue}var b=unescape(g[0]);var f=unescape(g[1]);f=f.replace(/\+/g," ");e[b]=f}return e}function tb_getPageSize(){var c=document.documentElement;var a=window.innerWidth||self.innerWidth||(c&&c.clientWidth)||document.body.clientWidth;var b=window.innerHeight||self.innerHeight||(c&&c.clientHeight)||document.body.clientHeight;arrayPageSize=[a,b];return arrayPageSize}function tb_detectMacXFF(){var a=navigator.userAgent.toLowerCase();
if(a.indexOf("mac")!=-1&&a.indexOf("firefox")!=-1){return true}}(function(a){a.fn.notifyme=function(r){var e=false;a("body").append('<div id="notify" class="notify">		<div id="dialog" title="Notify Me!">				<form>					<div id="dialog-content">							<p id="validateTips">Coming Soon.</p>					<fieldset>						<label for="part">Part #</label> 						<select id="part-select" class="ui-widget-content ui-corner-all"></select> 						<label for="qty">Requested Qty</label><br />						<input type="text" name="qty" id="qty" value="1" class="ui-widget-content ui-corner-all" /><br />						<label for="name">Name</label>						<input type="text" name="name" id="name" class="text ui-widget-content ui-corner-all" />						<input type="hidden" name="name" id="mfg" />						<div id="divEmail">							<label for="email">Email</label>							<input type="text" name="email" id="email" value="" class="text ui-widget-content ui-corner-all" />						</div>					</fieldset>					</div>				</form>					<div id="thankyou"><h3 class="black">Thank You.</h3><p>Your request has been successfully received.</p></div>			</div>		</div>');
var b=a("#name"),i=a("#dialog"),k=a("input#mfg"),c=a("#part-select"),j=a("#email"),g=a("#phone"),q=a("#qty"),m=a([]).add(b).add(j).add(c),f=a("#validateTips");function o(s){f.text(s).effect("highlight",{},1500)}function d(t){c.find("option").remove();var s=t.split("|");c.append(a("<option></option>").val("-").html("Select Part #"));a.each(s,function(v,u){c.append(a("<option></option>").val(u).html(u))})}function l(s,t){if(s.val().length==0){s.addClass("ui-state-error");o(t+" is a required field!");return false
}else{return true}}function p(s,t){if(s.val()=="-"){s.addClass("ui-state-error");o(t);return false}else{return true}}function h(t,s,u){if(!(s.test(t.val()))){t.addClass("ui-state-error");o(u);return false}else{return true}}a("input[name=via]").change(function(){if(a("input[name='via']:checked").val()=="email"){a("#divEmail").removeClass("hidden");a("#divPhone").addClass("hidden")}else{a("#divEmail").addClass("hidden");a("#divPhone").removeClass("hidden")}a(this).blur()});i.dialog({bgiframe:true,autoOpen:false,height:360,modal:true,buttons:{Submit:function(){var u=true;
var t="";m.removeClass("ui-state-error");u=u&&h(q,/^([0-9,])+$/i,"Quantity is invalid!");u=u&&p(a("select#part-select"),"Please select a part number!");u=u&&l(b,"Name");u=u&&h(j,/^((([a-z]|\d|[!#\$%&'\*\+\-\/=\?\^_`{\|}~]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])+(\.([a-z]|\d|[!#\$%&'\*\+\-\/=\?\^_`{\|}~]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])+)*)|((\x22)((((\x20|\x09)*(\x0d\x0a))?(\x20|\x09)+)?(([\x01-\x08\x0b\x0c\x0e-\x1f\x7f]|\x21|[\x23-\x5b]|[\x5d-\x7e]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(\\([\x01-\x09\x0b\x0c\x0d-\x7f]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF]))))*(((\x20|\x09)*(\x0d\x0a))?(\x20|\x09)+)?(\x22)))@((([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*([a-z]|\d|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))\.)+(([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])|(([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])([a-z]|\d|-|\.|_|~|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])*([a-z]|[\u00A0-\uD7FF\uF900-\uFDCF\uFDF0-\uFFEF])))\.?$/i,"eg. username@domain.com");
t=e?"&AutoNotification=1":"&AutoNotification=0";if(u){var v="listName=ComingSoon&PartNbr="+a("#part-select").val()+t+"&Title="+b.val()+"&Email="+j.val()+"&RequestedQTY="+q.val()+"&RequestTime="+Date("yyyy-MM-ddTHH:mm:ssZ")+"&Manufacturer="+k.val();var s={type:"GET",url:"/Ajax/SavePromoQue.ashx",data:v,success:function(w){n(w)}};a.ajax(s)}},Cancel:function(){a(this).dialog("close")}},close:function(){m.val("").removeClass("ui-state-error")}});function n(s){if(s=="Success"){setTimeout(function(){i.dialog("close");
a("#thankyou").hide();a("#dialog-content").show();a(".ui-dialog-buttonpane").show()},2000);a("#dialog-content").fadeOut(200).hide();a("#thankyou").fadeIn(200).show();a(".ui-dialog-buttonpane").fadeOut(200).hide()}}a(".btn-notify").click(function(){i.dialog("open");f.text(a(this).attr("title"));e=a(this).hasClass("auto")?true:false;a("input#mfg").val(a(this).attr("title"));d(a(this).attr("rel"))})}})(jQuery);(function(a){a.fn.quickview=function(){a(this).wrap('<span class="quickviewwrap"></span>');a(this).after('<img id="qvBtn" class="qvBtn" src="/images/qvBtn.png" style="display: none;">');
a(this).next(".qvBtn").css("top",-19);a(this).next(".qvBtn").css("left",20);a(".qvBtn").wrap('<a class="thickbox"></a>');a(".quickviewwrap").hover(function(){a(this).find(".qvBtn").show()},function(){a(this).find(".qvBtn").hide()});a(this).each(function(){a(this).next("a.thickbox").attr("href","/QuickViewProdDetail.aspx?PartNum="+a(this).attr("rel")+"&IsPKC=true&keepThis=true&TB_iframe=true&height=375&width=550&QuickView=true")});a(".thickbox").click(function(){a("#TB_window",window.parent.document).css({top:"50%",left:"50%",margin:"-"+(a("#TB_window",window.parent.document).height()/2)+"px 0 0 -"+(a("#TB_window",window.parent.document).width()/2)+"px"})
})}})(jQuery);