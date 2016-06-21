<?php

use Ecoli\Response;

class MainController
{
	/**
	 * Retrieve index.
	 *
	 * @return Ecoli\Response
	 */
	public function getIndex()
	{
		$content = "<h1>Ecoli</h1>";
		$content .= "<hr />";
		$content .= "<i>Ecoli " . FW_VERSION . " " . date('Y') . "</i>";

		return new Response($content);
	}

}