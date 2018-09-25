// Todo: get masks
maskNames = ["Test"]
maskFullNames = ["test"]

// Handle context menu
browser.contextMenus.onClicked.addListener(function (info) {
    if (maskFullNames.includes(info.menuItemId)) {
		browser.tabs.update({url:info.linkUrl});
	}
});

// build context menu
browser.contextMenus.create({
	id: "pm-root",
	title: "Open Link with PM",
	contexts: ["link"]
});
for(i = 0; i < maskNames.length; i++) {
	browser.contextMenus.create({
		id: maskFullNames[i],
		title: maskNames[i],
		parentId: "pm-root"
	});
}
