(function($){
	jQuery.fn.imgMoveScroll = function(){
		$(this).attr("ondragstart","return false;");
		$(this).mousedown(function(e){
			$(this).css("cursor","move");
			var X = e.pageX;
			var Y = e.pageY;
			var scrollX = $(window).scrollLeft();
			var scrollY = $(window).scrollTop();
			$(this).mousemove(function(e){
				$(window).scrollLeft(scrollX+((e.pageX - X)/1.5));
				$(window).scrollTop(scrollY+((e.pageY - Y)/1.5));
			});
		});
		$(this).mouseup(function(){
			$(this).css("cursor","pointer");
			$(this).unbind("mousemove");
		});
	};
})(jQuery);
