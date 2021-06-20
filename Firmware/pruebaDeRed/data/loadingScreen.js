function fadeOutEffect(targetId) {
    var fadeTarget = document.getElementById(targetId);
    var fadeEffect = setInterval(function () {
        if (!fadeTarget.style.opacity) {
            fadeTarget.style.opacity = 1;
        }
        if (fadeTarget.style.opacity > 0) {
            fadeTarget.style.opacity -= 0.1;
        } else {
            clearInterval(fadeEffect);
        }
    }, 50);
}

function fadeOutInEffect(targetOutId,targetInId) {
    var fadeTargetOut = document.getElementById(targetOutId);
		var fadeTargetIn = document.getElementById(targetInId);
		var quantityInterval = 0.1;
    var fadeEffect = setInterval(function () {
			if (!fadeTargetOut.style.opacity) {
					fadeTargetOut.style.opacity = 1;
			}else if (fadeTargetOut.style.opacity > 0) {
					fadeTargetOut.style.opacity = parseFloat(fadeTargetOut.style.opacity)-quantityInterval;
      }
			if (!fadeTargetIn.style.opacity) {
					fadeTargetIn.style.opacity = 0;
			}else if (fadeTargetIn.style.opacity < 1) {
          fadeTargetIn.style.opacity = parseFloat(fadeTargetIn.style.opacity)+quantityInterval;
      }

			if (fadeTargetOut.style.opacity == 0 && fadeTargetIn.style.opacity == 1) {
          clearInterval(fadeEffect);
      }
    }, 50);
}
